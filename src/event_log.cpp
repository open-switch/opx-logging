/*
 * Copyright (c) 2016 Dell Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * THIS CODE IS PROVIDED ON AN *AS IS* BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS
 * FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT.
 *
 * See the Apache Version 2.0 License for specific language governing
 * permissions and limitations under the License.
 */

/*
 * filename: event_log.cpp
 */

#include "event_log.h"
#include "event_log_types.h"

#include <stdarg.h>
#include <stdio.h>
#include <syslog.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include <mutex>
#include <type_traits>
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>
#include <systemd/sd-journal.h>

/*Future home can be determined but we have a general default and can be overridden by environmental variables*/
/*maybe take this path from the make environment in the near future*/
#define _LOG_CONFIG_FILE "/etc/opx/evlog.cfg"

static const char *_log_file_name = _LOG_CONFIG_FILE;
static pthread_once_t event_log_lib_inited = PTHREAD_ONCE_INIT;

static std::unordered_map<std::string,std::vector<bool>> * _mods;
static std::recursive_mutex *_mutex;

static size_t _min_level;
static size_t _max_level;

static time_t _last_signal;
static time_t _last_loaded=-1;

static bool _enable_all=true;

template <typename T>
inline void __fix_level(T &level) {
    if (level <_min_level|| level>=_max_level) {
        level = _max_level;
    }
}

inline void __fix_module(const char* &mod) {
    if (mod==nullptr) mod = "";
}


static void _reload_config(int sig) {
    time(&_last_signal);
}


void event_log_reload_config(){
    _reload_config(0);
}

#include <sys/signal.h>

void event_log_init() {
    std::vector<size_t> _levels =  {EV_T_LOG_EMERG,EV_T_LOG_ALERT,EV_T_LOG_CRIT,EV_T_LOG_ERR,
        EV_T_LOG_WARNING,EV_T_LOG_NOTICE,EV_T_LOG_INFO,EV_T_LOG_DEBUG };

    std::sort(_levels.begin(),_levels.end());
    _min_level = _levels.front();
    _max_level = _levels.back();

    _mods = new std::remove_reference<decltype(*_mods)>::type();
    _mutex = new std::recursive_mutex;

    struct sigaction _a;
    memset(&_a,0,sizeof(_a));
    _a.sa_handler = &_reload_config;

    //cam't do much if it fails but really there should be no valid failure case
    ///TODO Need to remove SIGUSR1 in a follow up update - temporary until SIGHUP totally integrated
    (void)sigaction(SIGUSR1,&_a,nullptr);

    (void)sigaction(SIGHUP,&_a,nullptr);

    //support Systemd environment override
    const char * _file_name = getenv("EV_LOGGING_CFG");
    if (_file_name!=nullptr) _log_file_name = _file_name;
}

inline void evt_check_log_init(){
    pthread_once(&event_log_lib_inited,event_log_init);
}

static std::vector<bool> &__get_mask(const char *mod) {
    std::lock_guard<std::recursive_mutex> lg(*_mutex);
    auto it = _mods->find(mod);
    if (it==_mods->end()) {
        std::vector<bool> &_flags = (*_mods)[mod];
        _flags.resize(_max_level+1);
        for ( auto  it : _flags ) {
            it = true;
        }
        _flags[EV_T_LOG_INFO] = false;
        _flags[EV_T_LOG_DEBUG] = false;
        return _flags;
    }
    return it->second;
}

void event_logging_set_enabled_flag(const char *mod,size_t loglvl, bool how) {
    evt_check_log_init();
    std::lock_guard<std::recursive_mutex> lg(*_mutex);
    __fix_level(loglvl);
    __fix_module(mod);

    std::vector<bool> &_flags = __get_mask(mod);
    _flags[loglvl] = how;
}

static inline const char * __adjust_mod_name(const char *mod) {
    const char *_tmpl = "ev_log_t_";
    while (*mod==*_tmpl) { ++mod,++_tmpl; }
    return mod;
}

 bool event_logging_is_enabled(const char *mod,size_t loglvl) {
    evt_check_log_init();

    std::lock_guard<std::recursive_mutex> lg(*_mutex);

    mod = __adjust_mod_name(mod);
    if (_last_loaded!=_last_signal) {
        _last_loaded = _last_signal;
        FILE *_fp = fopen(_log_file_name,"r");
        if (_fp!=nullptr) {
            _mods->clear();
            char _buff[256];
            while (fgets(_buff,sizeof(_buff)-1,_fp)!=nullptr) {
                char *_sep = strchr(_buff,' ');
                if (_sep==nullptr) continue;
                if (_sep==_buff) continue; //empty/invalid
                if (strlen(_buff)==0) continue; //invalid/empty

                *_sep = '\0';
                if (*(++_sep)==0) continue;

                //Overall log enable/disable
                if (strcmp(_buff,"ALL")==0) {
                    _enable_all = atoi(_sep);
                    continue;
                }

                bool _valid_line = false;

                //determine logging level
                size_t _level = atoi(_sep);
                bool _enable = false;

                //determine enable/disable
                _sep = strchr(_sep,' ');
                if (_sep!=nullptr ) {
                    ++_sep;
                    if (*_sep!=0) {
                        _enable = atoi(_sep);
                        _valid_line = true;
                    }
                }

                if (!_valid_line) continue;

                __fix_level(_level);
                event_logging_set_enabled_flag(_buff,_level,_enable);
            }
            fclose(_fp);
        }
    }

    if (!_enable_all) return false;

    __fix_module(mod);
    __fix_level(loglvl);


    /* Check if the sublevel and module id are in the valid enum range
     * loglvl is guaranteed to be the safe since user can not control its value
     */

    std::vector<bool> &_flags = __get_mask(mod);
    return _flags[loglvl];
}

bool event_log_msg(const char *module, int level, int ident, const char *file,
            const char * ln, const char *fun, const char *tag, int err, const char *msg,...) {
    va_list lst;
    va_start(lst,msg);

    if (ident == ALM_ID) { /* Alarm Support */
        /****
         * The following code was added to allow alarm manager retireve events pertaining to
         * alarm event. After design discussions, it was  decided that event_log_msg will be
         * morphed to  suit params required for ALM. Therefore, the original  param list may
         * be used in different context in the following code.
         */
        va_list args_cpy;
        va_copy (args_cpy, lst);
        const auto sz = std::vsnprintf (nullptr, 0, msg, lst)+1; //Extra byte allocated

        try {
            std::string detail_desc (sz, '\0');
            std::vsnprintf (&detail_desc.front(), sz, msg, args_cpy);
            va_end (args_cpy);

            sd_journal_send ("PRIORITY=%i", LOG_ALERT, /* alarm entries have higher precedence */
                    "TAG=alarm",                               /* Filtering tag */
                    tag, module,fun,detail_desc.c_str(),       /* Log Message */
                    "EV_NAME=%s", module,                      /* Name of the alarm event */
                    "EV_DESC=%s", fun,                         /* Description from alarm event */
                    "EV_SEV=%i", level,                        /* Alarm event severity */
                    "EV_ID=%i", err,                           /* Alarm event id */
                    "EV_SRC=%s", file,                         /* Alarm event source */
                    "EV_ACTION=%s", ln,                        /* Alarm event action */
                    "EV_DET_DESC=%s", detail_desc.c_str(),     /* Detail desciption from user */
                    NULL);
        } catch (const std::bad_alloc&) {
            va_end (args_cpy);
            va_end(lst);
            return false;
        }
    } else {
        evt_check_log_init();
        sd_journal_printv_with_location(level, file,ln,fun, msg,lst);
    }

    va_end(lst);
    return true;
}


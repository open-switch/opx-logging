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
 * filename: event_log.h
 */

#ifndef __EVENT_LOG_H
#define __EVENT_LOG_H

#include "event_log_types.h"

#include <syslog.h>
#include <errno.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


#include "event_log_types.h"


/**
 * @deprecated THIS API HAS BEEN DEPRECATED, PLEASE DO NOT USE THIS
 *
 * @note Please use @sa EV_LOGGING API instead
 *
 * @brief trace a message to through syslog
 *
 * @param MOD a module from event_log_types.h
 * @param a logging level (0-3)
 * @param a event id (private id for trace logs
 * @param message a message (printf formatted.. no return char please)
 * @param parameters for the message in standard printf format
 */
#define EV_LOG_TRACE(MOD, LVL, ID, message, ... ) \
    (void)((event_log_is_enabled(MOD,EV_T_LOG_DEBUG,LVL)) && \
        event_log_msg(MOD,EV_T_LOG_DEBUG,LVL,"[%s:%s]:%s:%s:%d, " message,    \
            event_log_mod_to_name(MOD),ID,__FILE__,__func__,__LINE__,##__VA_ARGS__))

/**
 * @deprecated THIS API HAS BEEN DEPRECATED, PLEASE DO NOT USE THIS
 *
 * @note Please use @sa EV_LOGGING API instead
 *
 * @brief log an error event.  may generate traps or the like...
 *
 * @param MOD a module from event_log_types.h
 * @param a event id (private id for trace logs
 * @param message a message (printf formatted.. no return char please)
 * @param parameters for the message in standard printf format
 */
#define EV_LOG_ERR(MOD, LVL, ID, message, ... ) \
    (void)((event_log_is_enabled(MOD,EV_T_LOG_ERR,LVL)) && \
        event_log_msg(MOD,EV_T_LOG_ERR, LVL,"[%s:%s], " message,    \
            event_log_mod_to_name(MOD),ID,##__VA_ARGS__))

/**
 * @deprecated THIS API HAS BEEN DEPRECATED, PLEASE DO NOT USE THIS
 *
 * @note Please use @sa EV_LOGGING API instead
 *
 * @brief info message
 *
 * @param MOD a module from event_log_types.h
 * @param a event id (private id for trace logs
 * @param message a message (printf formatted.. no return char please)
 * @param parameters for the message in standard printf format
 */
#define EV_LOG_INFO(MOD, LVL, ID, message, ... ) \
    (void)((event_log_is_enabled(MOD,EV_T_LOG_INFO,LVL)) && \
        event_log_msg(MOD,EV_T_LOG_INFO,LVL,"[%s:%s]:%s:%s:%d, " message,    \
            event_log_mod_to_name(MOD),ID,__FILE__,__func__,__LINE__,##__VA_ARGS__))

/**
 * @deprecated THIS API HAS BEEN DEPRECATED, PLEASE DO NOT USE THIS
 *
 * @note Please use @sa EV_LOGGING API instead
 *
 * @brief This is a wrapper for all of the above macros that will simplify the calling
 * @param what can be INFO, TRACE, ERR, DEBUG, CON_TRACE
 * @param MOD is the module concatenated with ev_log_t_ to create the real subsystem
 * @param LVL is a numeric level from 0-100 that indicates severity.  For error logs
 *             not applied at this point
 * @param ID is the logging ID that the message is tagged with in the files -
 * @param msg the message and parameters will follow this data
 */
#define EV_LOG( what, MOD, LVL, ID, msg, ...) \
    EV_LOG_##what(ev_log_t_ ## MOD, LVL,ID, msg ,##__VA_ARGS__)

/**
 * @deprecated THIS API HAS BEEN DEPRECATED, PLEASE DO NOT USE THIS
 *
 * @note Please use @sa EV_LOGGING API instead
 *
 * @brief log the errno to the error log
 *
 * @param MOD a module from event_log_types.h
 * @param a event id (private id for trace logs
 * @param ERRNO is the errno that you want printed to the log
 */
#define EV_LOG_ERRNO(MOD, LVL, ID, ERRNO) \
    (void)((event_log_is_enabled(MOD,EV_T_LOG_ERR,LVL)) && \
        event_log_msg_errno(MOD,EV_T_LOG_ERR,LVL,ERRNO, event_log_mod_to_name(MOD),ID,\
        __FILE__,__func__,__LINE__))

/**
 * @brief log the msg
 *
 * @param MOD a module from event_log_types.h
 * @param LVL a syslog level to be used for logging from event_log_types.h
 * @param ID  a event id (private id for trace logs)
 * @param msg the message and parameters will follow this data
 */
#define EV_LOGGING(MOD,LVL,ID,msg, ...) \
    (void)((event_logging_is_enabled(ev_log_t_ ## MOD, EV_T_LOG_ ## LVL)) && \
        (( EV_T_LOG_ ## LVL == EV_T_LOG_DEBUG) ? \
        event_log_msg(ev_log_t_ ## MOD,EV_T_LOG_ ## LVL,0,"[%s:%s]:%s:%s:%d, " msg,    \
                event_log_mod_to_name(ev_log_t_ ## MOD),ID,__FILE__,__func__,__LINE__,##__VA_ARGS__): \
        event_log_msg(ev_log_t_ ## MOD,EV_T_LOG_ ## LVL, 0,"[%s:%s], " msg,    \
                event_log_mod_to_name(ev_log_t_ ## MOD),ID,##__VA_ARGS__)))


/**
 * @brief change all logging state (enable or disable)
 *
 * @param state is to enable (true) or disable (false) all logging state
 *
 */
void event_log_change_all_state(bool state);

/**
 * @brief checks if logging is enabled for a given module, log-level and sub-level
 *
 * @param mod is a module from event_log_types.h
 * @param loglvl is the numeric log-level from event_log_types.h
 * @param sublvl is the numeric log-sub-level from event_log_types.h
 *
 * @return current logging status for given parameters (true/false)
 */
bool event_log_is_enabled(unsigned int sid,unsigned int loglvl,unsigned int sublvl);


/**
 * @brief checks if logging is enabled for a given module and log-level
 *
 * @param mod is a module from event_log_types.h
 * @param loglvl is the numeric log-level from event_log_types.h
 *
 * @return current logging status for given parameters (true/false)
 */
bool event_logging_is_enabled(unsigned int sid,unsigned int loglvl);


/**
 * @brief converts module id to string
 *
 * @param mod is a module from event_log_types.h
 *
 * @return string name of the corresponding module
 */
const char * event_log_mod_to_name(int mod);

/**
 * @brief logs the msg to the syslog
 *
 * @param mod is a module from event_log_types.h
 * @param level is the numeric log-level from event_log_types.h
 * @param sublvl is the numeric log-sub-level from event_log_types.h
 * @param msg is the message which needs to be logged
 *
 */
bool event_log_msg(int mod, int level, int sublvl, const char *msg, ...);


/**
 * @brief logs the error msg to the syslog
 *
 * @param mod is a module from event_log_types.h
 * @param level is the numeric log-level from event_log_types.h
 * @param sublvl is the numeric log-sub-level from event_log_types.h
 * @param err is the errno
 * @param sub is the string name of the module
 * @param id is the private ID to trace the log
 * @param fn is the file name on which the error occurred
 * @param func is the function name on which the error occurred
 * @param line the line number on which the error occurred
 *
 */
bool event_log_msg_errno(int mod, int level, int sublvl, int err,
    const char *sub, const char *id, const char *fn, const char*func, int line);


#ifdef __cplusplus
}
#endif

#endif

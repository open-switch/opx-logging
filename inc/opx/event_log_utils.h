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
 * filename: event_log_utils.h
 */

#ifndef EVENT_LOG_UTILS_H
#define EVENT_LOG_UTILS_H

#include "event_log_types.h"
#include <stdbool.h>

#define MAX_MODULES ev_log_t_MAX
#define MAX_LEVEL (MAX_LOG_LEVEL+1)
#define MAX_SUBLVL ev_log_s_MAX

typedef struct {
    bool status[MAX_SUBLVL];
}lvl_log_status_t;

typedef struct {
    lvl_log_status_t lvl_log_status[MAX_LEVEL];
}sys_log_status_t;

typedef unsigned int log_level_t;

#ifdef __cplusplus
extern "C" {
#endif

log_level_t * event_logging_get_default_state(void);
sys_log_status_t * event_log_get_default_state(void);
int event_log_get_default_state_size(void);
unsigned int event_logging_get_default_state_size(void);

void evt_check_log_init(void);
void event_log_init_shm(sys_log_status_t *ptr);
void event_logging_init_shm(log_level_t *ptr);

sys_log_status_t * event_log_get_shared_mem_pointer(void);
log_level_t * event_logging_get_shared_mem_pointer(void);

const char * event_log_lvl_to_name(int id);
const char * event_log_sublvl_to_name(int id);

void event_log_show_id(void);
void event_log_write_config(void);
void event_log_load_config(void);

void event_log_print_status(int sid, int lvl, int sublvl);
void event_log_set_status(unsigned int mod,unsigned int level);
void event_log_change_status(int sys, int lvl, int sublvl, bool stat);

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

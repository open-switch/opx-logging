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
 * filename: event_log.c
 */

#include "event_log.h"
#include "event_log_types.h"
#include "event_log_utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <syslog.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>


#define LOG_BUFFER_SIZE 100
#define SHARED_LOG_MEM "shared_log_mem"
#define SHARED_LOGGING_MEM "shared_logging_mem"

static pthread_once_t event_log_lib_inited = PTHREAD_ONCE_INIT;
static int ev_log_console_fd = -1;
static FILE * ev_log_console_fp = NULL;
static int ev_log_shm_fd = -1;
static int ev_logging_shm_fd = -1;
static sys_log_status_t *ev_log_mmap_ptr = NULL;
static log_level_t *ev_logging_mmap_ptr = NULL;


static log_level_t * event_logging_get_shared_struct(bool init){
    log_level_t *lvl_log_status_ptr = event_logging_get_default_state();
    if(init){
        if (ftruncate(ev_logging_shm_fd,
                      event_logging_get_default_state_size()) == -1) {
            EV_LOGGING(INFRA_COMMON, ERR, "LOGGING",
                       "ftruncate() failed, errno %s", strerror(errno));
        }
    }
    if(ev_logging_shm_fd != -1) {
        void *mmap_ptr = mmap(NULL,event_logging_get_default_state_size(),PROT_READ|PROT_WRITE,
                                MAP_SHARED, ev_logging_shm_fd, 0);
        if(mmap_ptr == NULL){
            return lvl_log_status_ptr;
        }

        if(init){
            event_logging_init_shm((log_level_t *)mmap_ptr);
        }

        lvl_log_status_ptr = (log_level_t *) (mmap_ptr);
    }
    return lvl_log_status_ptr;
}


static sys_log_status_t * event_log_get_shared_struct_pointer(bool init) {
    sys_log_status_t *sys_log_status_ptr = event_log_get_default_state();
    if(init){
        if (ftruncate(ev_log_shm_fd,
                      event_log_get_default_state_size()) == -1) {
            EV_LOGGING(INFRA_COMMON, ERR, "LOGGING",
                       "ftruncate() failed, errno %s", strerror(errno));
        }
    }
    if(ev_log_shm_fd != -1) {
        void *mmap_ptr = mmap(NULL,event_log_get_default_state_size(),PROT_READ|PROT_WRITE,
                                MAP_SHARED, ev_log_shm_fd, 0);
        if(mmap_ptr == NULL)    return sys_log_status_ptr;
        if(init) event_log_init_shm((sys_log_status_t *)mmap_ptr);
        sys_log_status_ptr = (sys_log_status_t *) (mmap_ptr);
    }
    return sys_log_status_ptr;
}


sys_log_status_t * event_log_get_shared_mem_pointer(){
    return ev_log_mmap_ptr;
}


log_level_t * event_logging_get_shared_mem_pointer(){
    return ev_logging_mmap_ptr;
}


static void event_log_init_shm_region() {
    ev_log_shm_fd = shm_open(SHARED_LOG_MEM,O_RDWR | O_CREAT | O_EXCL ,S_IRWXU);
    bool init = (ev_log_shm_fd != -1);
    if(ev_log_shm_fd == -1) {
        ev_log_shm_fd = shm_open(SHARED_LOG_MEM,O_RDWR | O_CREAT ,S_IRWXU);
        if (ev_log_shm_fd != -1){
            struct stat st;
            fstat(ev_log_shm_fd, &st);
            if((st.st_size) != event_log_get_default_state_size()) {
                init = true;
            }
        }
    }
    ev_log_mmap_ptr = event_log_get_shared_struct_pointer(init);
    fchmod(ev_log_shm_fd,S_IROTH |  S_IWOTH | S_IRUSR  | S_IWUSR );
    if (init) event_log_load_config();
}


static void event_logging_init_shm_region() {
    ev_logging_shm_fd = shm_open(SHARED_LOGGING_MEM,O_RDWR | O_CREAT | O_EXCL ,S_IRWXU);
    bool init = (ev_logging_shm_fd != -1);
    if(ev_logging_shm_fd == -1) {
        ev_logging_shm_fd = shm_open(SHARED_LOGGING_MEM,O_RDWR | O_CREAT ,S_IRWXU);
        if (ev_logging_shm_fd != -1){
            struct stat st;
            fstat(ev_logging_shm_fd, &st);
            if((st.st_size) != event_logging_get_default_state_size()) {
                init = true;
            }
        }
    }
    ev_logging_mmap_ptr = event_logging_get_shared_struct(init);
    fchmod(ev_logging_shm_fd,S_IROTH |  S_IWOTH | S_IRUSR  | S_IWUSR);
}


void event_log_init() {
    ev_log_console_fd = dup(STDOUT_FILENO);
    ev_log_console_fp = fdopen(ev_log_console_fd, "w");
    openlog(NULL, LOG_NOWAIT | LOG_NDELAY, LOG_USER);
    event_logging_init_shm_region();
    event_log_init_shm_region();
}

void evt_check_log_init(){
    pthread_once(&event_log_lib_inited,event_log_init);
}

bool event_log_is_enabled(unsigned int sid,unsigned int loglvl,unsigned int sublvl) {
    evt_check_log_init();
    /* Check if the sublevel and module id are in the valid enum range
     * loglvl is guaranteed to be the safe since user can not control its value
     */
    if( (sid < ev_log_t_MAX) && (sublvl < ev_log_s_MAX)){
        return ev_log_mmap_ptr[sid].lvl_log_status[loglvl].status[sublvl];
    }
    return false;
}

 bool event_logging_is_enabled(unsigned int sid,unsigned int loglvl) {
    evt_check_log_init();
    /* Check if the sublevel and module id are in the valid enum range
     * loglvl is guaranteed to be the safe since user can not control its value
     */
    if( sid < ev_log_t_MAX){
        return loglvl <= ev_logging_mmap_ptr[sid];
    }
    return false;
}

bool event_log_msg(int sys, int level ,int sublvl, const char *msg, ...) {
    va_list lst;
    va_start(lst,msg);
    vsyslog(level,msg,lst);
    va_end(lst);
    return true;
}


bool event_log_msg_errno(int sys, int level, int sublvl, int err,
    const char *sub, const char * id, const char *fn, const char*func, int line ) {
    char log_buffer[LOG_BUFFER_SIZE];
    if (strerror_r(err,log_buffer,sizeof(log_buffer))!=0) {
        snprintf(log_buffer,sizeof(log_buffer),"Unknown errno passed");
    }
    event_log_msg(sys,level,sublvl,"[%s:%d]:%s:%s:%d, %s",
        sub,id,fn,func,line,log_buffer);
    return true;
}


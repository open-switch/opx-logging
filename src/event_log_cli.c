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
 * filename: event_log_cli.c
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include "event_log_utils.h"



typedef void (*event_log_cli_wrapper)(int argc, char const * argv[]);

typedef struct {
    const char * command_name;
    event_log_cli_wrapper event_log_cli_fn;
}event_log_cli_t;

void event_log_show_id_wrapper(int argc, const char *argv[]){
    event_log_show_id();
}

void event_log_print_status_wrapper(int argc, const char *argv[]){
    if(argc<5){
        return;
    }
    event_log_print_status(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));
}

void event_log_enable_status_wrapper(int argc, const char *argv[]){
    if(argc<5){
        return;
    }
    event_log_change_status(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),true);
    event_log_write_config();
}

void event_log_set_status_wrapper(int argc, const char *argv[]){
    if(argc<4){
        return;
    }
    event_log_set_status(atoi(argv[2]),atoi(argv[3]));
    event_log_write_config();
}

void event_log_disable_status_wrapper(int argc, const char *argv[]){
    if(argc<5){
        return;
    }
    event_log_change_status(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),false);
    event_log_write_config();
}

void event_log_load_wrapper(int argc, const char *argv[]){
    event_log_load_config();
}

event_log_cli_t event_log_cli_commands[] = {
    { "info",event_log_show_id_wrapper },
    { "load",event_log_load_wrapper },
    { "show",event_log_print_status_wrapper },
    { "enable",event_log_enable_status_wrapper },
    { "disable",event_log_disable_status_wrapper },
    { "set",event_log_set_status_wrapper },
};

int main(int argc, char const *argv[])
{
    if(argc < 2){
        return 0;
    }
    evt_check_log_init();
    int ix;

    for( ix = 0 ; ix < sizeof(event_log_cli_commands)/sizeof(event_log_cli_commands[0]) ; ++ix){
        if ( strncmp(argv[1],event_log_cli_commands[ix].command_name, strlen(argv[1])) == 0) {
            event_log_cli_commands[ix].event_log_cli_fn(argc,argv);
        }
    }
    return 0;
}

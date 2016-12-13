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
 * filename: event_log_utils.c
 */

#include "event_log_utils.h"

#include "event_log_types.h"
#include "event_log.h"
#include <stdio.h>
 #include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define LOGGING_CFG_DIR         "/etc/opt/dell/os10/logging"
#define LOGGING_CFG_FILE         LOGGING_CFG_DIR"/config.xml"

static int log_level_list [] = {
EV_T_LOG_DEBUG,
EV_T_LOG_ERR,
EV_T_LOG_INFO,
};


static inline void event_log_print_mod_str(int sys) {
    printf("%-20s\t",event_log_mod_to_name(sys));
}

static inline void event_log_print_lvl_str(int lvl) {
    printf("%s \n    |\n",event_log_lvl_to_name(lvl));
}

static inline void event_log_print_sublvl_str(int sys, int lvl, int sublvl,sys_log_status_t *ptr) {
    printf("%d|",ptr[sys].lvl_log_status[lvl].status[sublvl]);
}

static void event_logging_print_header() {
    printf("\nModule\t\t\t");
    printf("level\t");
    printf("\n");
}

static void event_logging_print_status_all(log_level_t *ptr) {
    size_t mod_ix;
    event_logging_print_header();
    for(mod_ix = 0 ; mod_ix <  MAX_MODULES ; ++mod_ix) {
        event_log_print_mod_str(mod_ix);
        printf("%s",event_log_lvl_to_name(ptr[mod_ix]));
        printf("\n");
    }
    printf("\n");
}


static inline void event_logging_print_status_mod(unsigned int mod_ix,log_level_t *ptr) {
    event_logging_print_header();
    event_log_print_mod_str(mod_ix);
    printf("%s",event_log_lvl_to_name(ptr[mod_ix]));
    printf("\n");
    printf("\n\n");
}


static inline void event_logging_change_status_all(bool status,log_level_t *sys_log_status_ptr) {
    size_t mod_ix;
    for(mod_ix = 0 ; mod_ix <  MAX_MODULES ; ++mod_ix) {
        if(status){
            sys_log_status_ptr[mod_ix] = EV_T_LOG_DEBUG;
        }else{
            sys_log_status_ptr[mod_ix] = EV_T_LOG_EMERG;
        }
    }
}


static inline void event_logging_change_status_mod(bool status,unsigned int mod_ix,
                                            log_level_t *sys_log_status_ptr) {
    if(status){
        sys_log_status_ptr[mod_ix] = EV_T_LOG_DEBUG;
    }else{
        sys_log_status_ptr[mod_ix] = EV_T_LOG_EMERG;
    }
}


static void event_log_print_header(int lvl) {
    size_t lvl_ix;
    printf("\n X|X|X|X| corresponds to ");
    for (lvl_ix = 0 ; lvl_ix < MAX_SUBLVL ; ++lvl_ix){
        printf("%s|", event_log_sublvl_to_name(lvl_ix));
    }
    printf("\n\n    Module\t\t");
    if (lvl == -1){
        for( lvl_ix = 0 ; lvl_ix < sizeof(log_level_list)/sizeof(log_level_list[0]) ; ++lvl_ix) {
            printf("%s\t\t",event_log_lvl_to_name(log_level_list[lvl_ix]));
        }
    }
    else {
        printf("%s\t\t",event_log_lvl_to_name(lvl));
    }
    printf("\n");
}


void event_log_set_status(unsigned int mod,unsigned int level){
    log_level_t *lvl_ptr = event_logging_get_shared_mem_pointer();
    lvl_ptr[mod] = level;
}


static void event_log_print_status_all(sys_log_status_t *ptr) {
    size_t mod_ix,lvl_ix,sublvl_ix;
    event_log_print_header(-1);
    for(mod_ix = 0 ; mod_ix <  MAX_MODULES ; ++mod_ix) {
        event_log_print_mod_str(mod_ix);
        for( lvl_ix = 0 ; lvl_ix < sizeof(log_level_list)/sizeof(log_level_list[0]) ; ++lvl_ix) {
            for(sublvl_ix =0 ; sublvl_ix < MAX_SUBLVL ; ++sublvl_ix) {
                event_log_print_sublvl_str(mod_ix,log_level_list[lvl_ix],sublvl_ix,ptr);
            }
            printf("\t");
        }
        printf("\n");
    }
    printf("\n");
}


static void event_log_print_status_mod(int sys, sys_log_status_t *ptr) {
    size_t lvl_ix,sublvl_ix;
    event_log_print_header(-1);
    event_log_print_mod_str(sys);
    for( lvl_ix = 0 ; lvl_ix < sizeof(log_level_list)/sizeof(log_level_list[0]) ; ++lvl_ix) {
        for(sublvl_ix =0 ; sublvl_ix < MAX_SUBLVL ; ++sublvl_ix) {
            event_log_print_sublvl_str(sys,log_level_list[lvl_ix],sublvl_ix,ptr);
        }
        printf("\t");
    }
    printf("\n\n");
}


static void event_log_print_status_lvl(int sys, int lvl, sys_log_status_t *ptr ) {
    size_t sublvl_ix;
    event_log_print_header(lvl);
    event_log_print_mod_str(sys);
    for(sublvl_ix =0 ; sublvl_ix < MAX_SUBLVL ; ++sublvl_ix) {
        event_log_print_sublvl_str(sys,lvl,sublvl_ix,ptr);
    }
    printf("\n\n");
}


static void event_log_print_status_sublvl(int sys, int lvl, int sublvl, sys_log_status_t *ptr ) {
    printf("\n");
    event_log_print_header(lvl);
    event_log_print_mod_str(sys);
    event_log_print_sublvl_str(sys,lvl,sublvl,ptr);
    printf("\n\n");
}


void event_log_print_status(int sys, int lvl, int sublvl) {
    sys_log_status_t *ptr = event_log_get_shared_mem_pointer();
    log_level_t *lvl_ptr = event_logging_get_shared_mem_pointer();
    if(sublvl == -1) {
        if(lvl == -1) {
            if(sys == -1) {
                event_log_print_status_all(ptr);
                event_logging_print_status_all(lvl_ptr);
            }
            else {
                event_log_print_status_mod(sys, ptr);
                event_logging_print_status_mod(sys, lvl_ptr);
            }
        }
        else {
            event_log_print_status_lvl(sys, lvl, ptr);
        }
    }
    else{ event_log_print_status_sublvl(sys, lvl, sublvl, ptr); }
}


static void event_log_change_status_all(bool status, sys_log_status_t *sys_log_status_ptr) {
    size_t mod_ix,lvl_ix,sublvl_ix;
    for(mod_ix = 0 ; mod_ix <  MAX_MODULES ; ++mod_ix) {
        for( lvl_ix = 0 ; lvl_ix < sizeof(log_level_list)/sizeof(log_level_list[0]) ; ++lvl_ix) {
            for(sublvl_ix = 0 ; sublvl_ix < MAX_SUBLVL ; ++sublvl_ix) {
                sys_log_status_ptr[mod_ix].lvl_log_status[log_level_list[lvl_ix]].status[sublvl_ix]=status;
            }
        }
    }
}


static void event_log_change_status_mod(int sys, bool status, sys_log_status_t *sys_log_status_ptr){
    size_t lvl_ix,sublvl_ix;
    for( lvl_ix = 0 ; lvl_ix < sizeof(log_level_list)/sizeof(log_level_list[0]) ; ++lvl_ix) {
        for(sublvl_ix =0 ; sublvl_ix < MAX_SUBLVL ; ++sublvl_ix) {
            sys_log_status_ptr[sys].lvl_log_status[log_level_list[lvl_ix]].status[sublvl_ix]=status;
        }
    }

}


static void event_log_change_status_lvl(int sys, int lvl,bool status, sys_log_status_t *sys_log_status_ptr) {
    size_t sublvl_ix;
    for(sublvl_ix =0 ; sublvl_ix < MAX_SUBLVL ; ++sublvl_ix) {
        sys_log_status_ptr[sys].lvl_log_status[lvl].status[sublvl_ix]=status;
    }
}


void event_log_change_status(int system, int level, int sublevel, bool status) {
    sys_log_status_t *ptr = event_log_get_shared_mem_pointer();
    log_level_t *lvl_ptr = event_logging_get_shared_mem_pointer();
    if( sublevel == -1) {
        if(level == -1) {
            if(system == -1) {
                event_log_change_status_all(status, ptr);
                event_logging_change_status_all(status,lvl_ptr);
            }
            else {
                event_log_change_status_mod(system,status, ptr);
                event_logging_change_status_mod(status,system,lvl_ptr);
            }
        }
        else {
            event_log_change_status_lvl(system,level,status, ptr);
        }
    }
    else{ ptr[system].lvl_log_status[level].status[sublevel]=status; }
}


void event_log_change_all_state(bool state) {
    evt_check_log_init();
    event_log_change_status(-1, -1, -1, state);
}


int event_log_name_to_mod(const char *name, int len) {
    int i;
    const char *mod_name;
    for (i = 0; i < MAX_MODULES; i++) {
        mod_name = event_log_mod_to_name(i);
        if (len != strlen(mod_name)) {
            continue;
        }
        if (!strncasecmp(name,mod_name,len)) {
            return i;
        }
    }
    return -1;
}


int event_log_lvl_to_val(const char *name, int len) {
    int i;
    for (i=0; i < MAX_LEVEL; i++) {
        if (!strncasecmp(name, event_log_lvl_to_name(i), len)) {
            return i;
        }
    }
    return -1;
}


void event_log_write_config()
{
    FILE *fp;
    size_t mod_ix,lvl_ix,sublvl_ix;
    sys_log_status_t *ptr = event_log_get_shared_mem_pointer();
    log_level_t *lvl_ptr = event_logging_get_shared_mem_pointer();
    struct stat  f_stat;

    /* Directory doesnt exist? - create */
    if (stat(LOGGING_CFG_DIR, &f_stat) == -1) {
        mkdir(LOGGING_CFG_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }

    fp = fopen(LOGGING_CFG_FILE, "w+");

    if (!fp) return;

    fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
    fprintf(fp, "<logging>\r\n");

    fprintf(fp, "  <old_style>>\r\n");
    for(mod_ix = 0 ; mod_ix <  MAX_MODULES ; ++mod_ix) {
        fprintf(fp, "\t<module name=\"%s\">\r\n", event_log_mod_to_name(mod_ix));
        for( lvl_ix = 0 ; lvl_ix < sizeof(log_level_list)/sizeof(log_level_list[0]) ; ++lvl_ix) {
            fprintf(fp, "\t\t<%s ", event_log_lvl_to_name(log_level_list[lvl_ix]));
            for(sublvl_ix =0 ; sublvl_ix < MAX_SUBLVL ; ++sublvl_ix) {
                fprintf(fp, "%s=\"%d\" ", event_log_sublvl_to_name(sublvl_ix),
                    ptr[mod_ix].lvl_log_status[log_level_list[lvl_ix]].status[sublvl_ix]);
            }
            fprintf(fp, " />\r\n");
        }
        fprintf(fp, "\t</module>\r\n");
    }

    fprintf(fp, "  </old_style>>\r\n");
    fprintf(fp, "  <log_levels>\r\n");

    for(mod_ix = 0 ; mod_ix <  MAX_MODULES ; ++mod_ix) {
        fprintf(fp, "\t<module name=\"%s\"", event_log_mod_to_name(mod_ix));

        fprintf(fp, "\t level=\"%s\" />\r\n", event_log_lvl_to_name(lvl_ptr[mod_ix]));
    }
    fprintf(fp, " </log_levels>\r\n");
    fprintf(fp, "</logging>\r\n");

    fclose(fp);
}


static inline char *config_attr_get(xmlNode * node, const char *attr)
{
    return (char *)xmlGetProp((xmlNode *)node, (xmlChar *)attr);
}


void event_log_load_config()
{
    xmlDoc      *cfg_hdl;
    const char          *name;
    char                *attrib;
    xmlNode     *root, *node, *child;
    int         sys, lvl, state;
    int         sublvl_ix;
    struct stat  f_stat;

    /* File doesnt exist? */
    if (stat(LOGGING_CFG_FILE, &f_stat) == -1) {
        return;
    }

    cfg_hdl = xmlReadFile(LOGGING_CFG_FILE, NULL, 0);
    if (cfg_hdl == NULL) {
        return;
    }

    root = xmlDocGetRootElement(cfg_hdl);
    root = root->children;

    if (!root || !root->next) return;

    root = root->next;

    node = root->children;
    for (; node != NULL; node = node->next) {
        if (node->type != XML_ELEMENT_NODE) continue;

        name = (char*)node->name;
        if (strncmp(name, "module", strlen(name))) continue;;

        attrib = config_attr_get(node, "name");
        if (!attrib || ((sys = event_log_name_to_mod(attrib, strlen(attrib))) == -1)) {
            continue;
        }

        child = node->children;
        if (!child) continue;

        for (; child != NULL; child=child->next) {
            if (child->type != XML_ELEMENT_NODE) continue;

            name = (char*)child->name;
            if (!name || ((lvl = event_log_lvl_to_val(name, strlen(name))) == -1)) {
                continue;
            }

            for(sublvl_ix =0 ; sublvl_ix < MAX_SUBLVL ; ++sublvl_ix) {
                attrib = config_attr_get(child, event_log_sublvl_to_name(sublvl_ix));
                if (!attrib) {
                    continue;
                }
                state = atoi(attrib);
                event_log_change_status(sys, lvl, sublvl_ix, state);
            }
        }
    }

    root = root->next;

    if (!root || !root->next) return;
    root = root->next->children;

    if (!root || !root->next) return;
    node = root->next;

    for (; node != NULL; node = node->next) {
        if (node->type != XML_ELEMENT_NODE) continue;

        name = (char*)node->name;
        if (strncmp(name, "module", strlen(name))) continue;;

        attrib = config_attr_get(node, "name");
        if (!attrib || ((sys = event_log_name_to_mod(attrib, strlen(attrib))) == -1)) {
            continue;
        }

        attrib = config_attr_get(node, "level");
        if (!attrib || ((lvl = event_log_lvl_to_val(attrib, strlen(attrib))) == -1)) {
            continue;
        }

        event_log_set_status(sys, lvl);
    }

    xmlFreeDoc(cfg_hdl);
    xmlCleanupParser();
}


void event_log_init_shm(sys_log_status_t *ptr) {
    size_t mod_ix,lvl_ix,sublvl_ix;
    for( mod_ix = 0 ; mod_ix <  MAX_MODULES ; ++mod_ix) {
        for( lvl_ix = 0 ; lvl_ix < MAX_LEVEL ; ++lvl_ix) {
            for(sublvl_ix =0 ; sublvl_ix < MAX_SUBLVL ; ++sublvl_ix) {
                if(lvl_ix <= EV_T_LOG_ERR){
                    ptr[mod_ix].lvl_log_status[lvl_ix].status[sublvl_ix] = true;
                }
                else{
                    if((lvl_ix == EV_T_LOG_INFO) && (sublvl_ix == 0)) {
                        ptr[mod_ix].lvl_log_status[lvl_ix].status[sublvl_ix] = true;
                    } else {
                        ptr[mod_ix].lvl_log_status[lvl_ix].status[sublvl_ix] = false;
                    }
                }
            }
        }
    }
}


void event_logging_init_shm(log_level_t *ptr) {
    size_t mod_ix;
    for( mod_ix = 0 ; mod_ix <  MAX_MODULES ; ++mod_ix) {
        ptr[mod_ix] = EV_T_LOG_NOTICE;
    }

}


void event_log_show_id()
{
    size_t mod_ix;
    printf("Module IDs\n");
    printf("========================================\n");
    for( mod_ix = 0 ; mod_ix <  MAX_MODULES ; ++mod_ix)
        printf("%-15s \t\t %lu\n",event_log_mod_to_name(mod_ix), mod_ix);
    printf("\nLog Level IDs\n");
    printf("========================================\n");
    for( mod_ix = 0 ; mod_ix < MAX_LEVEL ; ++mod_ix) {
        const char *str = event_log_lvl_to_name(mod_ix);
        if(*str) printf("%-15s \t\t %lu\n",str,mod_ix);
    }
    printf("\nLog Sub Level IDs\n");
    printf("========================================\n");
    for(mod_ix =0 ; mod_ix < MAX_SUBLVL ; ++mod_ix) {
        printf("%-15s \t\t %lu\n",event_log_sublvl_to_name(mod_ix), mod_ix);
    }
    printf("\n");
}


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

#include <python2.7/Python.h>

#include "event_log.h"

extern int event_log_name_to_mod(const char *name, int len);

static PyObject *
log_trace(PyObject *self, PyObject *args)
{
    int        lvl, linenum;
    const char *modname, *id, *filename, *funcname, *mesg;

    if (!PyArg_ParseTuple(args, "sisssis", &modname, &lvl, &id, &filename, &funcname, &linenum, &mesg))  return (NULL);

    int mod = event_log_name_to_mod(modname, strlen(modname));
    if (mod < 0)  return (NULL);

    if (event_log_is_enabled(mod, EV_T_LOG_DEBUG, lvl)) {
        event_log_msg(mod, EV_T_LOG_DEBUG, lvl, "[%s:%s]:%s:%s:%d, %s",
                      modname, id, filename, funcname, linenum, mesg
                      );
    }

    return (Py_BuildValue(""));
}

static PyObject *
log_info(PyObject *self, PyObject *args)
{
    int        lvl, linenum;
    const char *modname, *id, *filename, *funcname, *mesg;

    if (!PyArg_ParseTuple(args, "sisssis", &modname, &lvl, &id, &filename, &funcname, &linenum, &mesg))  return (NULL);

    int mod = event_log_name_to_mod(modname, strlen(modname));
    if (mod < 0)  return (NULL);

    if (event_log_is_enabled(mod, EV_T_LOG_INFO, lvl)) {
        event_log_msg(mod, EV_T_LOG_INFO, lvl, "[%s:%s]:%s:%s:%d, %s",
                      modname, id, filename, funcname, linenum, mesg
                      );
    }

    return (Py_BuildValue(""));
}

static PyObject *
log_err(PyObject *self, PyObject *args)
{
    int        lvl;
    const char *modname, *id, *mesg;

    if (!PyArg_ParseTuple(args, "siss", &modname, &lvl, &id, &mesg))  return (NULL);

    int mod = event_log_name_to_mod(modname, strlen(modname));
    if (mod < 0)  return (NULL);

    if (event_log_is_enabled(mod, EV_T_LOG_ERR, lvl)) {
        event_log_msg(mod, EV_T_LOG_ERR, lvl,"[%s:%s], %s",
                      modname, id, mesg
                      );
    }

    return (Py_BuildValue(""));
}

static PyObject * logging(PyObject *self, PyObject *args)
{
    int        lvl, linenum;
    const char *modname, *id, *filename, *funcname, *mesg;

    if (!PyArg_ParseTuple(args, "sisssis", &modname, &lvl, &id, &filename, &funcname, &linenum, &mesg))  return (NULL);


    int mod = event_log_name_to_mod(modname, strlen(modname));
    if (mod < 0)  return (NULL);

    if (event_logging_is_enabled(mod, lvl)) {
        if(lvl >= EV_T_LOG_INFO){
            event_log_msg(mod, lvl, lvl, "[%s:%s]:%s:%s:%d, %s",
                          modname, id, filename, funcname, linenum, mesg);
        }else{
            event_log_msg(mod, lvl, lvl,"[%s:%s], %s",
                          modname, id, mesg);
        }
    }

    return (Py_BuildValue(""));
}

static PyMethodDef methods[] = {
    { "log_trace", log_trace, METH_VARARGS, "Log trace" },
    { "log_info",  log_info,  METH_VARARGS, "Log info" },
    { "log_err",   log_err,   METH_VARARGS, "Log error" },
    { "logging",   logging,   METH_VARARGS, "Log msg" },
    { NULL, NULL, 0, NULL }        /* Sentinel */
};

PyMODINIT_FUNC
initevent_log(void)
{
    PyObject *m = Py_InitModule("event_log", methods);

    PyModule_AddIntConstant(m, "EMERG", EV_T_LOG_EMERG);
    PyModule_AddIntConstant(m, "ALERT", EV_T_LOG_ALERT);
    PyModule_AddIntConstant(m, "CRIT", EV_T_LOG_CRIT);
    PyModule_AddIntConstant(m, "ERR", EV_T_LOG_ERR);
    PyModule_AddIntConstant(m, "WARNING", EV_T_LOG_WARNING);
    PyModule_AddIntConstant(m, "NOTICE", EV_T_LOG_NOTICE);
    PyModule_AddIntConstant(m, "INFO", EV_T_LOG_INFO);
    PyModule_AddIntConstant(m, "DEBUG", EV_T_LOG_DEBUG);}

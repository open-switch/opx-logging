#
# Copyright (c) 2019 Dell Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may
# not use this file except in compliance with the License. You may obtain
# a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
#
# THIS CODE IS PROVIDED ON AN *AS IS* BASIS, WITHOUT WARRANTIES OR
# CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
# LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS
# FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT.
#
# See the Apache Version 2.0 License for specific language governing
# permissions and limitations under the License.
#
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([opx-logging],[2.2.0],[ops-dev@lists.openswitch.net])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIRS([m4])
AC_CHECK_HEADERS([libxml/parser.h])

# Checks for programs.
AC_PROG_CXX
AM_PATH_PYTHON
AC_PROG_CC
LT_INIT([shared])
# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h syslog.h unistd.h libxml2/libxml/parser.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MMAP
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([ftruncate mkdir strncasecmp])

AC_CONFIG_FILES([Makefile inc/Makefile])
AC_OUTPUT

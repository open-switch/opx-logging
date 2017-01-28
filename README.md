# opx-logging
This repository contains logging utilities for the OPX project which provide a simple wrapper for syslog activities. These wrapper functions and macros add the line number of the calling code, as well as other additional pieces of information.

You can optionally use the CLI for run-time modification of logging levels called _opx_logging_cli_.

All headers are located in OPX, and will be installed in the _*/usr/include/opx*_ folder (based on your prefix).

##Packages
Contained within this repository are 

libopx-logging1_<version>_<arch>.deb    - Utility libraries
libopx-logging-dev_<version>_<arch>.deb - Exported header files
python-opx-logging_<version>_<arch>.deb - python bindings


See https://github.com/open-switch/opx-docs/wiki/Architecture for information on Logging module.

(c) 2017 Dell

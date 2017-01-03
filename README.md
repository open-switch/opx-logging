# opx-logging
This repository contains logging utilities for the OPX project which provide a simple wrapper for syslog activities. These wrapper functions and macros add the line number of the calling code, as well as other additional pieces of information.

You can optionally use the CLI for run-time modification of logging levels called _opx_logging_cli_.

All headers are located in OPX, and will be installed in the _*/usr/include/opx*_ folder (based on your prefix).

## Build
See [opx-nas-manifest](https://github.com/open-switch/opx-nas-manifest) for complete information on common build tools.

### Build dependencies

None

(c) Dell 2017

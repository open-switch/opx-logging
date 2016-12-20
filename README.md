# opx-logging

## Description
This repo contains logging utilities for the OPX project which provide a simple wrapper for syslog activities. These wrapper functions and macros add the line
number of the calling code, as well as other additional pieces of information.

You can optionally use the CLI for runtime modification of logging levels called `opx_logging_cli`.

All headers are located in OPX, and will be installed in the */usr/include/opx* folder (based on your prefix).

## Build
See [opx-nas-manifest](https://github.com/open-switch/opx-nas-manifest) for complete information on common build tools.

### Build dependencies
None

BUILD CMD: sonic_build  -- clean binary

(c) Dell 2016

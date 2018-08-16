# Logging

This repository contains logging utilities for the OPX project which provide a simple wrapper for syslog activities. These wrapper functions and macros add the line number of the calling code, as well as other additional pieces of information.

You can optionally use the CLI for run-time modification of logging levels called `opx_logging_cli`.

All headers are located in OPX, and will be installed in the `*/usr/include/opx*` folder (based on your prefix).

## Packages

- `libopx-logging1` — Utility libraries  
- `libopx-logging-dev` — Exported header files
- `python-opx-logging` — Python bindings

See [Architecture](https://github.com/open-switch/opx-docs/wiki/Architecture) for more information on the Logging module.

(c) 2018 Dell Inc. or its subsidiaries. All Rights Reserved.

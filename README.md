# OpenSwitch Logging

These logging utilities for the OPX project which provide a simple wrapper for syslog activities. These wrapper functions and macros add the line number of the calling code, as well as other additional pieces of information.

You can optionally use the CLI for run-time modification of logging levels with `opx_logging_cli`.

All headers are located in OPX, and will be installed in the `/usr/include/opx` folder (based on the prefix you set).

## Packages

- `libopx-logging1_version_arch.deb` — Utility libraries
- `libopx-logging-dev_version_arch.deb` — Exported header files
- `python-opx-logging_version_arch.deb` — Python bindings

See [Architecture](https://github.com/open-switch/opx-docs/wiki/Architecture) for more information on the logging module.

(c) 2017 Dell

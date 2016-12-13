from distutils.core import setup, Extension

setup(name = 'event_log',
      version = '1.0',
      description = 'Interface to Base event log service',
      ext_modules = [Extension('event_log',
                               sources = ['event_log.c'],
                               libraries=['event_log']
                               )
                    ]
      )

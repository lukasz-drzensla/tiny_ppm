# Overview

Please bear in mind this software is in early development stage.

tiny_ppm is an open source library that implements the PPM file format, based on specification: https://netpbm.sourceforge.net/doc/ppm.html
Its objective is to provide a small yet functional library for creating, storing and manipulating pictures.

# Building
## Required tools
* A C compiler such as gcc or clang
* GNU Make or its ported version

While developing gcc version 11.4.0 and GNU Make version 4.3 were used.

## Compiling
Clone the repository to a directory. Open the directory in terminal or other command line tool. Then execute the following command (replace `<platform>` with a platform specified in Makefile, eg `linux` or `windows`):

`make PLATFORM=<platform>`

If you are not intending to use built-in filters and effects, you can exclude it from compilation by specifying `FILTERS=no` option.
Example command for building the library for linux with effects excluded:

`make PLATFORM=linux FILTERS=no`

Binaries will be placed in an automatically created `build` directory in the main project directory. Executable for example application is named `ppm` or `ppm.exe` (when building for Windows). Before executing, make sure the binary file has correct access permissions.

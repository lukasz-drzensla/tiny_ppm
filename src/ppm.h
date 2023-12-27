/* 
* Copyright (C) 2023  Lukasz Drzensla Ldrzensla@yahoo.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/*
* Tiny PPM is an open source implementation of Portable Pixel Map
* Based on specification from: https://netpbm.sourceforge.net/doc/ppm.html 
*/

#ifndef _PPM_H
#define _PPM_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void ppm_init_logger(void *restrict _logger_stream);

/*perfectly fine changing these parameters*/
enum {
    COMMENT_LINE_LEN = 256,
    FILEPATH_MAX_LEN = 255,
    BUF_MAX_LEN = 65536,
    LINE_MAX_LEN = 256
};

#define DEF_COMMENT_STR "Created by TinyPPM"

/*DO NOT change these parameters*/
typedef enum PPM_STATUS_t {
    PPM_OK = 0,
    PPM_ERR_IO = 1,
    PPM_ERR_PARAM = 2,
    PPM_ERR_GEN = 3,
    PPM_NOT_INIT = 4
} PPM_STATUS_t;

enum {
    MAGIC_NUMBER_LEN = 2,    
    UINT16_STR_LEN = 5,
    WHITESPACE_LEN = 1 
};

typedef enum ppm_magic
{
    P3 = 0, //Plain PPM format
    P6 = 1 //PPM format
} ppm_magic;

struct ppm_pixel {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
};

struct ppm_image{
    /* Numbers in comments resemble the numbers from 'THE FORMAT' paragraph from https://netpbm.sourceforge.net/doc/ppm.html */
    ppm_magic magic_number; /*1*/
    char initial_comment [COMMENT_LINE_LEN+1];
    uint16_t width; /*3*/
    uint16_t height; /*5*/
    uint16_t maxval; /*7*/
    struct ppm_pixel** raster; /*9*/
    /*End of specification section*/

    unsigned char _initialised;
    char filepath[FILEPATH_MAX_LEN];

    PPM_STATUS_t (*set_filepath)(struct ppm_image* ppm_image, const char* filepath);
    PPM_STATUS_t (*destroy_raster)(struct ppm_image* ppm_image);
    PPM_STATUS_t (*init_raster)(struct ppm_image* ppm_image);
    PPM_STATUS_t (*save_to_file)(struct ppm_image* ppm_image);
    PPM_STATUS_t (*set_pixel)(struct ppm_image* ppm_image, const int row, const int col, const uint16_t red, const uint16_t green, const uint16_t blue);
    PPM_STATUS_t (*load_from_file)(struct ppm_image* ppm_image);
};

struct ppm_image ppm_create(const ppm_magic magic_numer, const char* initial_comment, const uint16_t width, const uint16_t height, const uint16_t maxval);
struct ppm_image ppm_create_empty(void);

#endif /* _PPM_H */

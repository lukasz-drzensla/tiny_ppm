#ifndef _PPM_H
#define _PPM_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef enum PPM_STATUS_t {
    PPM_OK = 0,
    PPM_ERR_IO = 1,
    PPM_ERR_PARAM = 2,
    PPM_ERR_GEN = 3
} PPM_STATUS_t;

/*DO NOT change these parameters*/
enum {
    MAGIC_NUMBER_LEN = 2,    
    UINT16_STR_LEN = 5,
    WHITESPACE_LEN = 1 
};

/*perfectly fine changing these parameters*/
enum {
    COMMENT_LINE_LEN = 256,
    FILEPATH_MAX_LEN = 255,
    BUF_MAX_LEN = 65536,
    LINE_MAX_LEN = 256
};

typedef enum ppm_magic
{
    P3 = 0,
    P6 = 1
} ppm_magic;

struct ppm_pixel {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
};

struct ppm_image{
    /*Numbers in comments resemble the numbers from 'THE FORMAT' paragraph from https://netpbm.sourceforge.net/doc/ppm.html*/
    ppm_magic magic_number; /*1*/
    char initial_comment [COMMENT_LINE_LEN+1];
    uint16_t width; /*3*/
    uint16_t height; /*5*/
    uint16_t maxval; /*7*/
    struct ppm_pixel** raster; /*9*/
    
    /*End of specification section*/

    char filepath[FILEPATH_MAX_LEN];

    PPM_STATUS_t (*set_filepath)(struct ppm_image* ppm_image, const char* filepath);
    PPM_STATUS_t (*destroy_raster)(struct ppm_image* ppm_image);
    PPM_STATUS_t (*init_raster)(struct ppm_image* ppm_image);
    PPM_STATUS_t (*create_buffer)(struct ppm_image* ppm_image, char* dest_buffer);
    PPM_STATUS_t (*save_to_file)(struct ppm_image* ppm_image);
    PPM_STATUS_t (*set_pixel)(struct ppm_image* ppm_image, const int row, const int col, const uint16_t red, const uint16_t green, const uint16_t blue);
    PPM_STATUS_t (*load_from_buffer)(struct ppm_image* ppm_image, char* buffer);
    PPM_STATUS_t (*load_from_file)(struct ppm_image* ppm_image);
};

struct ppm_image ppm_create(const ppm_magic magic_numer, const char* initial_comment, const uint16_t width, const uint16_t height, const uint16_t maxval);
#endif /*_PPM_H*/

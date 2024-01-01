/*
* Copyright (C) 2023-2024  Lukasz Drzensla Ldrzensla@yahoo.com
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
enum
{
    COMMENT_LINE_LEN = 256,
    FILEPATH_MAX_LEN = 255,
    BUF_MAX_LEN = 65536,
    LINE_MAX_LEN = 256
};

#define DEF_COMMENT_STR "Created by TinyPPM"
#define IGN_WARN TRUE

/*DO NOT change these parameters*/
/**
 * @brief Return type for functions used to manipulate images. Warnings can co-exist with PPM_OK - to ignore warnings check only the 0-th bit or set IGN_WARN to TRUE.
 */
typedef enum PPM_STATUS_t
{
    PPM_OK = 1,
    PPM_ERR_IO = 2,
    PPM_ERR_PARAM = 4,
    PPM_ERR_GEN = 8,
    PPM_ERR_NOT_INIT = 16,
    PPM_WARN_TRIM = 32
} PPM_STATUS_t;

enum
{
    MAGIC_NUMBER_LEN = 2,
    UINT16_STR_LEN = 5,
    WHITESPACE_LEN = 1
};

typedef enum ppm_magic
{
    P3 = 0, // Plain PPM format
    P6 = 1  // PPM format
} ppm_magic;

struct ppm_pixel
{
    uint16_t red;
    uint16_t green;
    uint16_t blue;
};

/**
 * @brief This structure contains full information about an image.
*/
struct ppm_image
{
    /* Specifiaction section */
    /* Numbers in comments resemble the numbers from 'THE FORMAT' paragraph from https://netpbm.sourceforge.net/doc/ppm.html */
    ppm_magic magic_number; /*1*/
    char initial_comment[COMMENT_LINE_LEN + 1];
    uint16_t width;            /*3*/
    uint16_t height;           /*5*/
    uint16_t maxval;           /*7*/
    struct ppm_pixel **raster; /*9*/
    /*End of specification section*/

    /**
     * @brief A boolean value indicating whether the memory for a raster has been allocated. Mainly used internally.
     */
    unsigned char _initialised;

    /**
     * @brief Path to the file.
     */
    char filepath[FILEPATH_MAX_LEN];

    /**
     * @brief Set the filepath to be used when loading and saving to a file.
     */
    PPM_STATUS_t (*set_filepath)(struct ppm_image *ppm_image, const char *filepath);

    /**
     * @brief Frees the memory dynamically allocated for the image.
     */
    PPM_STATUS_t (*destroy_raster)(struct ppm_image *ppm_image);

    /**
     * @brief Dynamically allocate the memory for the image.
     */
    PPM_STATUS_t (*init_raster)(struct ppm_image *ppm_image);

    /**
     * @brief Save to file. Filepath must be set beforehand.
     */
    PPM_STATUS_t (*save_to_file)(struct ppm_image *ppm_image);

    /**
     * @brief Set the value of an individual pixel.
     */
    PPM_STATUS_t (*set_pixel)(struct ppm_image *ppm_image, const int row, const int col, const uint16_t red, const uint16_t green, const uint16_t blue);

    /**
     * @brief Load an image from a file. Filepath must be set beforehand.
     */
    PPM_STATUS_t (*load_from_file)(struct ppm_image *ppm_image);

    /**
     * @brief This function calculates the actual size of a ppm image. May differ from the final file size after saving to file by a byte (EOF).
     * @return Size of the ppm image (in bytes).
     */
    size_t (*get_size)(struct ppm_image *ppm_image);
};
/**
 * @brief Create an instance of a struct ppm_image. Bear in mind that this function does not initialise the raster! 
 */
struct ppm_image ppm_create(const ppm_magic magic_numer, const char *initial_comment, const uint16_t width, const uint16_t height, const uint16_t maxval);

/**
 * @brief Create an empty instance of a struct ppm_image. All values will be set to default. Bear in mind that this function does not initialise the raster! 
 */
struct ppm_image ppm_create_empty(void);

#endif /* _PPM_H */

/* 
* Copyright (C) 2023  Lukasz Drzensla
*/

/*
* This is an example application using the TinyPPM library
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ppm.h"
#include "ppm_filters.h"

int main (int argc, char** argv)
{
    ppm_init_logger(stderr);
    struct ppm_image image = ppm_create_empty();
    const char* filepath = "/home/lukasz/projects/file.ppm";
    image.set_filepath(&image, filepath);
    //PPM_STATUS_t res = image.load_from_file(&image); /* We do not need to know the exact format when loading an image - P3 or P6 */
    image.load_from_file(&image); /* We do not need to know the exact format when loading an image - P3 or P6 */

    // if (PPM_OK == res)
    // {
    //     ppm_filter_invert(&image);
    // }

    const char* filepath2 = "/home/lukasz/projects/output.ppm";
    image.set_filepath(&image, filepath2);
    image.magic_number = P6; /* Set the format to P3 = Plain PPM (aka ASCII PPM)*/
    image.maxval = 16;
    if (PPM_OK != image.save_to_file(&image)) /* This function will use magic_number from the structure to choose the format */
    {
        printf ("Error!\n");
    }

    image.destroy_raster(&image);
    return 0;
}

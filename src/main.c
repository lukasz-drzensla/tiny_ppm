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
    const char* filepath = "E:\\Projekty\\img_prot\\build\\file.ppm";
    image.set_filepath(&image, filepath);
    image.load_from_file(&image); /* We do not need to know the exact format when loading an image - P3 or P6 */
    
    ppm_filter_invert(&image);

    const char* filepath2 = "E:\\Projekty\\img_prot\\build\\output.ppm";
    image.set_filepath(&image, filepath2);
    image.magic_number = P3; /* Set the format to P3 = Plain PPM (aka ASCII PPM)*/
    if (PPM_OK != image.save_to_file(&image)) /* This function will use magic_number from the structure to choose the format */
    {
        printf ("Error!\n");
    }

    image.destroy_raster(&image);
    return 0;
}

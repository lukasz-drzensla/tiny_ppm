/* 
* Copyright (C) 2023  Lukasz Drzensla
*/

#include "ppm_filters.h"

PPM_STATUS_t ppm_filter_invert (struct ppm_image* ppm_image)
{
    PPM_STATUS_t retval = PPM_OK;
    for (int row = 0; row < ppm_image->height; row++)
    {
        for (int col = 0; col < ppm_image->width; col++)
        {
            retval = ppm_image->set_pixel(ppm_image, row, col, ppm_image->maxval - ppm_image->raster[row][col].red, ppm_image->maxval -  ppm_image->raster[row][col].green, ppm_image->maxval -  ppm_image->raster[row][col].blue);
        }
    }
    return retval;
}

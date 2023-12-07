#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ppm.h"

int main (int argc, char** argv)
{
    struct ppm_image image = ppm_create(P3, "Test image", 2, 2, 255);
    const char* filepath = "E:\\Projekty\\img_prot\\build\\file.ppm";
    image.set_filepath(&image, filepath);
    image.init_raster(&image);
    /*
    for (int row = 0; row < image.height; row++)
    {
        for (int col = 0; col < image.width; col++)
        {
           image.set_pixel(&image, row, col, 50*row, 50*col, 50*row + 50*col);
        }
    }
    image.save_to_file(&image);*/
    //char buffer[256] = "P3 #Test image\n2 2 255 255 0 0 255 0 0 255 0 0 255 0 0";
    //image.load_from_buffer(&image, buffer);
    
    image.load_from_file(&image);
    for (int row = 0; row < image.height; row++)
    {
        for (int col = 0; col < image.width; col++)
        {
            image.set_pixel(&image, row, col, image.maxval - image.raster[row][col].red, image.maxval -  image.raster[row][col].green, image.maxval -  image.raster[row][col].blue);
        }
    }
    const char* filepath2 = "E:\\Projekty\\img_prot\\build\\output.ppm";
    image.set_filepath(&image, filepath2);
    image.save_to_file(&image);

    image.destroy_raster(&image);
    return 0;
}

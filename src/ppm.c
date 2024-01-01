/*
 * Copyright (C) 2023-2024  Lukasz Drzensla
 */

#include "ppm.h"
#include "hlp_str.h"
#include "hlp_ppm.h"

PPM_STATUS_t ppm_set_filepath(struct ppm_image *ppm_image, const char *filepath);
PPM_STATUS_t ppm_destroy_raster(struct ppm_image *ppm_image);
PPM_STATUS_t ppm_init_raster(struct ppm_image *ppm_image);
PPM_STATUS_t ppm_save_to_file(struct ppm_image *ppm_image);
PPM_STATUS_t ppm_set_pixel(struct ppm_image *ppm_image, const int row, const int col, const uint16_t red, const uint16_t green, const uint16_t blue);
PPM_STATUS_t ppm_load_from_file(struct ppm_image *ppm_image);
size_t ppm_get_size(struct ppm_image* ppm_image);


PPM_STATUS_t ppm_write_hdr_section(char *buffer, FILE *ppm_file, struct ppm_image *ppm_image, int *cursor);
struct ppm_pixel **ppm_create_raster(struct ppm_image *ppm_image);

void _ppm_create(struct ppm_image *ppm_image);

#define ADD_RET_WARN(RETVAL, WARNING) do { if(!IGN_WARN) RETVAL |= WARNING; } while(0) 

FILE *logger_stream = NULL;

/**
 * @brief Redirect the logs to a stream. Useful for debugging the library.
*/
void ppm_init_logger(void *restrict _logger_stream)
{
    logger_stream = (FILE *)_logger_stream;
}

inline void _ppm_create(struct ppm_image *ppm_image)
{
    ppm_image->_initialised = FALSE;
    ppm_image->set_filepath = &ppm_set_filepath;
    ppm_image->destroy_raster = &ppm_destroy_raster;
    ppm_image->init_raster = &ppm_init_raster;
    ppm_image->save_to_file = &ppm_save_to_file;
    ppm_image->set_pixel = &ppm_set_pixel;
    ppm_image->load_from_file = &ppm_load_from_file;
    ppm_image->get_size = &ppm_get_size;
}

struct ppm_image ppm_create(const ppm_magic magic_numer, const char *initial_comment, const uint16_t width, const uint16_t height, const uint16_t maxval)
{
    struct ppm_image image;
    switch (magic_numer)
    {
    case P3:
    {
        image.magic_number = P3;
        break;
    }
    case P6:
    {
        image.magic_number = P6;
        break;
    }
    default:
    {
        image.magic_number = P3;
        break;
    }
    }
    strncpy(image.initial_comment, initial_comment, strlen(initial_comment) + 1);
    image.width = width;
    image.height = height;
    image.maxval = maxval;

    _ppm_create(&image);

    return image;
}

struct ppm_image ppm_create_empty(void)
{
    struct ppm_image image;
    image.magic_number = P6;
    const char *initial_comment = DEF_COMMENT_STR;
    strncpy(image.initial_comment, initial_comment, strlen(initial_comment) + 1);
    image.width = 1;
    image.height = 1;
    image.maxval = 255;
    _ppm_create(&image);
    return image;
}

PPM_STATUS_t ppm_set_filepath(struct ppm_image *ppm_image, const char *filepath)
{
    memset(ppm_image->filepath, 0, FILEPATH_MAX_LEN);
    strncpy(ppm_image->filepath, filepath, strlen(filepath) + 1);
    ppm_image->filepath[strlen(filepath)] = END_CHAR;
    return PPM_OK;
}

PPM_STATUS_t ppm_destroy_raster(struct ppm_image *ppm_image)
{
    if (FALSE == ppm_image->_initialised)
    {
        return PPM_ERR_NOT_INIT;
    }

    if (NULL == ppm_image)
    {
        return PPM_ERR_PARAM;
    }

    if (NULL == ppm_image->raster)
    {
        return PPM_ERR_PARAM;
    }

    for (int i = 0; i < ppm_image->height; i++)
    {
        free(ppm_image->raster[i]);
    }
    free(ppm_image->raster);
    ppm_image->_initialised = FALSE;
    return PPM_OK;
}

PPM_STATUS_t ppm_init_raster(struct ppm_image *ppm_image)
{
    ppm_image->raster = ppm_create_raster(ppm_image);
    ppm_image->_initialised = TRUE;
    return PPM_OK;
}

struct ppm_pixel **ppm_create_raster(struct ppm_image *ppm_image)
{
    struct ppm_pixel **raster;
    raster = (struct ppm_pixel **)calloc(ppm_image->height, sizeof(struct ppm_pixel *));
    for (int i = 0; i < ppm_image->height; i++)
    {
        raster[i] = (struct ppm_pixel *)calloc(ppm_image->width, sizeof(struct ppm_pixel));
    }
    return raster;
}

PPM_STATUS_t ppm_load_from_file(struct ppm_image *ppm_image)
{
    PPM_STATUS_t destroy_res = ppm_image->destroy_raster(ppm_image); /*width and height CAN change thus we need to free allocated memory beforehand*/
    if (destroy_res != PPM_OK && destroy_res != PPM_ERR_NOT_INIT)
    {
        return destroy_res;
    }

    FILE *ppm_file = NULL;
    ppm_file = fopen(ppm_image->filepath, "r");

    if (NULL == ppm_file)
    {
        fprintf(logger_stream, "Input file: %s not found\n", ppm_image->filepath);
        return PPM_ERR_PARAM;
    }

    char buffer[LINE_MAX_LEN];
    fgets(buffer, LINE_MAX_LEN, ppm_file);

    /*parse magic number*/
    if ('P' != buffer[0])
    {
        return PPM_ERR_PARAM;
    }

    if ('3' == buffer[1])
    {
        /*P3 version = ascii ppm*/
        ppm_image->magic_number = P3;
    }
    else if ('6' == buffer[1])
    {
        /*P6 version*/
        ppm_image->magic_number = P6;
        fclose(ppm_file);
        ppm_file = fopen(ppm_image->filepath, "rb");
        fgets(buffer, LINE_MAX_LEN, ppm_file);
    }
    else
    {
        return PPM_ERR_PARAM;
    }

    /*parse (ignore) initial comment*/
    fgets(buffer, LINE_MAX_LEN, ppm_file);
    if (COMMENT_CHAR == buffer[0])
    {
        fgets(buffer, LINE_MAX_LEN, ppm_file);
    }
    replace_all_whitespaces(buffer, WHITESPACE);
    int cursor = 0;
    cursor += get_next(buffer + cursor, &(ppm_image->width)) + 1; /*read width*/
    get_next(buffer + cursor, &(ppm_image->height));              /*read height*/
    fgets(buffer, LINE_MAX_LEN, ppm_file);
    get_next(buffer, &(ppm_image->maxval)); /*read maxval*/

    if (PPM_OK != ppm_image->init_raster(ppm_image))
    {
        return PPM_ERR_GEN;
    }

    if (P3 == ppm_image->magic_number)
    {
        int i = 0;
        int row = 0;
        int col = 0;
        while (fscanf(ppm_file, "%s", buffer) > 0)
        {
            if (COMMENT_CHAR == buffer[0])
            {
                continue;
            }
            if (ppm_image->width == col)
            {
                col = 0;
                row++;
            }
            switch (i)
            {
            case 0:
            {
                get_next(buffer, &(ppm_image->raster[row][col].red));
                break;
            }
            case 1:
            {
                get_next(buffer, &(ppm_image->raster[row][col].green));
                break;
            }
            case 2:
            {
                i = -1;
                get_next(buffer, &(ppm_image->raster[row][col].blue));
                col++;
                break;
            }
            default:
            {
                break;
            }
            }
            i++;
        }
    }
    else if (P6 == ppm_image->magic_number)
    {
        if (256 > ppm_image->maxval)
        {
            /* If maxval is less than 256 a sample is represented by 1 byte */
            int i = 0;
            int row = 0;
            int col = 0;
            int next = 0; /* next character read from the file */

            while (EOF != (next = fgetc(ppm_file)))
            {
                if (ppm_image->width == col)
                {
                    col = 0;
                    row++;
                    if (row == ppm_image->width)
                    {
                        break;
                    }
                }
                switch (i)
                {
                case 0:
                {
                    ppm_image->raster[row][col].red = next;
                    break;
                }
                case 1:
                {
                    ppm_image->raster[row][col].green = next;
                    break;
                }
                case 2:
                {
                    i = -1;
                    ppm_image->raster[row][col].blue = next;
                    col++;
                    break;
                }
                default:
                {
                    break;
                }
                }
                i++;
            }
        }
        else
        {
            /* If maxval is more than 256 a sample is represented by 2 bytes */
            int i = 0;
            int row = 0;
            int col = 0;
            int next_msb = 0; /* next character read from the file - MSB*/
            int next_lsb = 0; /* next character read from the file - MSB*/
            int next = 0;     /* sum of two bytes above */
            while (EOF != (next_msb = fgetc(ppm_file)))
            {
                if (EOF == (next_lsb = fgetc(ppm_file)))
                {
                    break;
                }
                next = next_lsb | (next_msb << 8);
                if (ppm_image->width == col)
                {
                    col = 0;
                    row++;
                    if (row == ppm_image->width)
                    {
                        break;
                    }
                }
                switch (i)
                {
                case 0:
                {
                    ppm_image->raster[row][col].red = next;
                    break;
                }
                case 1:
                {
                    ppm_image->raster[row][col].green = next;
                    break;
                }
                case 2:
                {
                    i = -1;
                    ppm_image->raster[row][col].blue = next;
                    col++;
                    break;
                }
                default:
                {
                    break;
                }
                }
                i++;
            }

            return PPM_OK;
        }
    }
    else
    {
        fprintf(logger_stream, "Error: unknown format: %d\n", (int)ppm_image->magic_number);
        return PPM_ERR_PARAM;
    }

    fclose(ppm_file);
    return PPM_OK;
}

PPM_STATUS_t ppm_write_hdr_section(char *buffer, FILE *ppm_file, struct ppm_image *ppm_image, int *cursor)
{
    if (0 > fputs(buffer, ppm_file))
    {
        return PPM_ERR_IO;
    }
    memset(buffer, 0, LINE_MAX_LEN);
    buffer[0] = COMMENT_CHAR;

    for (*cursor = 1; *cursor < 1 + strlen(ppm_image->initial_comment); (*cursor)++)
    {
        buffer[*cursor] = ppm_image->initial_comment[*cursor - 1];
    }
    buffer[(*cursor)++] = WHITESPACE;
    if (0 > fputs(buffer, ppm_file))
    {
        return PPM_ERR_IO;
    }
    memset(buffer, 0, LINE_MAX_LEN);
    *cursor = 0;
    insert_int_to_str(buffer + *cursor, ppm_image->width);
    *cursor += strlen_int(ppm_image->width);
    buffer[(*cursor)++] = ' '; /*Exceptionally used space instead of new line to comply with gimp format*/
    insert_int_to_str(buffer + *cursor, ppm_image->height);
    *cursor += strlen_int(ppm_image->height);
    buffer[(*cursor)++] = WHITESPACE;
    insert_int_to_str(buffer + (*cursor), ppm_image->maxval);
    *cursor += strlen_int(ppm_image->maxval);
    buffer[(*cursor)++] = WHITESPACE;
    if (0 > fputs(buffer, ppm_file))
    {
        return PPM_ERR_IO;
    }
    return PPM_OK;
}

PPM_STATUS_t ppm_save_to_file(struct ppm_image *ppm_image)
{
    if (FALSE == ppm_image->_initialised)
    {
        return PPM_ERR_NOT_INIT;
    }
    PPM_STATUS_t retval = PPM_OK;
    FILE *ppm_file = NULL;
    ppm_file = fopen(ppm_image->filepath, "w");
    if (NULL == ppm_file)
    {
        fprintf(logger_stream, "Error: cannot create file: %s\n", ppm_image->filepath);
        return PPM_ERR_IO;
    }

    char buffer[LINE_MAX_LEN];
    memset(buffer, 0, LINE_MAX_LEN);
    switch (ppm_image->magic_number)
    {
    case P3:
    {
        buffer[0] = 'P';
        buffer[1] = '3';
        buffer[2] = WHITESPACE;
        int cursor = 1;
        PPM_STATUS_t res = ppm_write_hdr_section(buffer, ppm_file, ppm_image, &cursor);
        if (PPM_OK != res)
        {
            return res;
        }

        for (int row = 0; row < ppm_image->height; row++)
        {
            for (int col = 0; col < ppm_image->width; col++)
            {
                memset(buffer, 0, LINE_MAX_LEN);
                cursor = 0;
                insert_int_to_str(buffer + cursor, ppm_image->raster[row][col].red);
                cursor += strlen_int(ppm_image->raster[row][col].red);
                buffer[cursor++] = WHITESPACE;

                insert_int_to_str(buffer + cursor, ppm_image->raster[row][col].green);
                cursor += strlen_int(ppm_image->raster[row][col].green);
                buffer[cursor++] = WHITESPACE;

                insert_int_to_str(buffer + cursor, ppm_image->raster[row][col].blue);
                cursor += strlen_int(ppm_image->raster[row][col].blue);
                buffer[cursor++] = WHITESPACE;

                if (0 > fputs(buffer, ppm_file))
                {
                    return PPM_ERR_IO;
                }
            }
        }
        memset(buffer, 0, LINE_MAX_LEN);
        buffer[0] = END_CHAR;
        if (0 > fputs(buffer, ppm_file))
        {
            return PPM_ERR_IO;
        }

        break;
    }
    case P6:
    {
        fclose(ppm_file);
        ppm_file = fopen(ppm_image->filepath, "wb");
        if (NULL == ppm_file)
        {
            fprintf(logger_stream, "Error: cannot create file: %s\n", ppm_image->filepath);
            return PPM_ERR_IO;
        }

        buffer[0] = 'P';
        buffer[1] = '6';
        buffer[2] = WHITESPACE;
        int cursor = 1;
        PPM_STATUS_t res = ppm_write_hdr_section(buffer, ppm_file, ppm_image, &cursor);
        if (PPM_OK != res)
        {
            return res;
        }

        if (ppm_image->maxval < 256)
        {
            /* single byte version*/
            char c = 0;
            for (int row = 0; row < ppm_image->height; row++)
            {
                for (int col = 0; col < ppm_image->width; col++)
                {
                    if (ppm_image->raster[row][col].red > ppm_image->maxval)
                    {
                        ADD_RET_WARN(retval, PPM_WARN_TRIM);
                        c = (char)(ppm_image->maxval);
                    }
                    else
                    {
                        c = (char)ppm_image->raster[row][col].red;
                    }
                    if (0 > fputc(c, ppm_file))
                    {
                        return PPM_ERR_IO;
                    }

                    if (ppm_image->raster[row][col].green > ppm_image->maxval)
                    {
                        ADD_RET_WARN(retval, PPM_WARN_TRIM);
                        c = (char)(ppm_image->maxval);
                    }
                    else
                    {
                        c = (char)ppm_image->raster[row][col].green;
                    }
                    if (0 > fputc(c, ppm_file))
                    {
                        return PPM_ERR_IO;
                    }

                    if (ppm_image->raster[row][col].blue > ppm_image->maxval)
                    {
                        ADD_RET_WARN(retval, PPM_WARN_TRIM);
                        c = (char)(ppm_image->maxval);
                    }
                    else
                    {
                        c = (char)ppm_image->raster[row][col].blue;
                    }
                    if (0 > fputc(c, ppm_file))
                    {
                        return PPM_ERR_IO;
                    }
                }
            }
        }
        else
        {
            /* two byte version*/
            char c = 0;
            for (int row = 0; row < ppm_image->height; row++)
            {
                for (int col = 0; col < ppm_image->width; col++)
                {
                    /* MSB */
                    c = (char)((ppm_image->raster[row][col].red & 0xFF00) >> 8);
                    if (0 > fputc(c, ppm_file))
                    {
                        return PPM_ERR_IO;
                    }
                    /* LSB */
                    c = (char)((ppm_image->raster[row][col].red & 0xFF));
                    if (0 > fputc(c, ppm_file))
                    {
                        return PPM_ERR_IO;
                    }

                    /* MSB */
                    c = (char)((ppm_image->raster[row][col].green & 0xFF00) >> 8);
                    if (0 > fputc(c, ppm_file))
                    {
                        return PPM_ERR_IO;
                    }
                    /* LSB */
                    c = (char)((ppm_image->raster[row][col].green & 0xFF));
                    if (0 > fputc(c, ppm_file))
                    {
                        return PPM_ERR_IO;
                    }

                    /* MSB */
                    c = (char)((ppm_image->raster[row][col].blue & 0xFF00) >> 8);
                    if (0 > fputc(c, ppm_file))
                    {
                        return PPM_ERR_IO;
                    }
                    /* LSB */
                    c = (char)((ppm_image->raster[row][col].blue & 0xFF));
                    if (0 > fputc(c, ppm_file))
                    {
                        return PPM_ERR_IO;
                    }
                }
            }
        }

        memset(buffer, 0, LINE_MAX_LEN);
        buffer[0] = END_CHAR;
        if (0 > fputs(buffer, ppm_file))
        {
            return PPM_ERR_IO;
        }

        break;
    }
    default:
    {
        return PPM_ERR_PARAM;
        break;
    }
    }

    fclose(ppm_file);
    return retval;
}

PPM_STATUS_t ppm_set_pixel(struct ppm_image *ppm_image, const int row, const int col, const uint16_t red, const uint16_t green, const uint16_t blue)
{
    ppm_image->raster[row][col].red = red;
    ppm_image->raster[row][col].green = green;
    ppm_image->raster[row][col].blue = blue;
    return PPM_OK;
}


/**
* @brief This function calculates the actual size of a ppm image. May differ from the final file size after saving to file by a byte (EOF).
* @return Size of the ppm image (in bytes).
*/
size_t ppm_get_size(struct ppm_image* ppm_image)
{
    size_t size = 0;
    size += sizeof(ppm_magic);
    size += strlen(ppm_image->initial_comment)*sizeof(char);
    size += ppm_image->maxval > 255 ? (3 * sizeof(uint16_t)) : (3 * sizeof(uint8_t));
    size += ppm_image->maxval > 255 ? (sizeof(struct ppm_pixel**) + ppm_image->width * ppm_image->height * sizeof(struct ppm_pixel)) : (sizeof(struct ppm_pixel**) + ppm_image->width * ppm_image->height * sizeof(struct ppm_pixel) / 2);
    return size;
}

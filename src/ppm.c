#include "ppm.h"

#define _HEADER_LEN  MAGIC_NUMBER_LEN + COMMENT_LINE_LEN + 3 * UINT16_STR_LEN + 5 * WHITESPACE_LEN
#define _RASTER_LEN (UINT16_STR_LEN + WHITESPACE_LEN) * 3
#define _BUFFER_LEN(image) _HEADER_LEN + _RASTER_LEN * image->width * image->height
#define WHITESPACE '\n'
#define END_CHAR '\0'
#define COMMENT_CHAR '#'

PPM_STATUS_t ppm_set_filepath(struct ppm_image* ppm_image, const char* filepath);
PPM_STATUS_t ppm_destroy_raster (struct ppm_image* ppm_image);
PPM_STATUS_t ppm_init_raster (struct ppm_image* ppm_image);
PPM_STATUS_t ppm_save_to_buffer (struct ppm_image* ppm_image, char* dest_buffer);
PPM_STATUS_t ppm_save_to_file (struct ppm_image* ppm_image);
PPM_STATUS_t ppm_set_pixel(struct ppm_image* ppm_image, const int row, const int col, const uint16_t red, const uint16_t green, const uint16_t blue);
PPM_STATUS_t ppm_load_from_buffer (struct ppm_image* ppm_image, char* buffer);
PPM_STATUS_t ppm_load_from_file (struct ppm_image* ppm_image);

struct ppm_pixel** ppm_create_raster (struct ppm_image* ppm_image);

void _ppm_init(struct ppm_image* ppm_image);

int _power (const int base, const int exp)
{
    int result = 1;
    for (int i = 0; i < exp; i++)
    {
        result *= base;
    }
    return result;
}

int _strlen_int (const int var)
{ 
    int count = 0;
    int _var = var;
    do { 
        _var /= 10; ++count;
    } while (_var != 0);
    return count;
}

int _insert_int_to_str (char* dest, const int var)
{
    int len = _strlen_int (var);
    for (int i = 0; i < len; i++)
    {
        dest[i] = (char) ((var / _power (10, len-i-1) % 10) + 48);
    }
    return len;
}

void _remove_comments (char *buffer)
{
  int found_comment = 0;
  int i = 0;
  while (END_CHAR != buffer[i])
    {
      if (found_comment)
	{
	  if (WHITESPACE == buffer[i])
	    {
	      found_comment = 0;
	    }
	  buffer[i] = COMMENT_CHAR;
	}
      else
	{
	  if (COMMENT_CHAR == buffer[i])
	    {
	      found_comment = 1;
	    }
	}
      i++;
    }
    char *p_read = buffer;
    char *p_write = buffer;
    while (END_CHAR != *p_read)
    {
      *p_write = *p_read++;
      p_write += (*p_write != COMMENT_CHAR);
    }
    *p_write = END_CHAR;
}

void _replace_all_whitespaces (char* buffer, const char c)
{
    int length = strlen(buffer);
    for (int i = 0; i < length; i++)
    {
        if ((' ' == buffer[i]) || ('\n' == buffer[i]) || ('\r' == buffer[i]) || ('\t' == buffer[i]))
        {
            buffer[i] = c;
        }
    }
}

void _replace_consecutive_with_single_another (char* buffer, const char c, const char another)
{
    int length = strlen (buffer);
    char new_buffer[length];
    int found_first = 0;
    int j = 0;
    for (int i = 0; i < length; i++)
    {
        if (c == buffer[i])
        {
            if (!found_first)
            {
                found_first = 1;
                new_buffer[j] = another;
                j++;
            }
        } else {
            found_first = 0;
            new_buffer[j] = buffer[i];
            j++;
        }
    }
    for (int i = 0; i < j; i++)
    {
        buffer[i] = new_buffer[i];
    }
    buffer[j] = END_CHAR;
}

/**
*@brief This function interprets the first word in a string as a 16-bit unsigned integer
*@param buffer a string that contains integers represented as digits eg 'test 123 test 456'
*@param dest a pointer to an integer where the result will be stored
*@return The number of characters processed - can be then used in next function call to move forward in the buffer and thus get next integer in a string
*/
int _get_next(char* buffer, uint16_t* dest)
{
    char temp_buf[UINT16_STR_LEN] = {0};
    uint16_t temp = 0;
    int i = 0;
    while ((*buffer) && (*buffer != WHITESPACE))
    {
        temp_buf[i] = *buffer;
        i++;
        buffer++;
    }
    for (int j = 0; j < i; j++)
    {
        temp += (((uint16_t)temp_buf[j]) - 48) * (uint16_t)_power(10, i - j - 1);
    }
    if (i != 0)
    {
        *dest = (uint16_t)temp;
    } else {
        i = 1;
        *dest = 0;
    }
    return i;
}

inline void _ppm_init(struct ppm_image* ppm_image)
{
    ppm_image->set_filepath = &ppm_set_filepath;
    ppm_image->destroy_raster = &ppm_destroy_raster;
    ppm_image->init_raster = &ppm_init_raster;
    ppm_image->create_buffer = &ppm_save_to_buffer;
    ppm_image->save_to_file = &ppm_save_to_file;
    ppm_image->set_pixel = &ppm_set_pixel;
    ppm_image->load_from_buffer = &ppm_load_from_buffer;
    ppm_image->load_from_file = &ppm_load_from_file;
}

struct ppm_image ppm_create(const ppm_magic magic_numer, const char* initial_comment, const uint16_t width, const uint16_t height, const uint16_t maxval)
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

    _ppm_init(&image);
    
    return image;
}

PPM_STATUS_t ppm_set_filepath(struct ppm_image* ppm_image, const char* filepath)
{
    memset(ppm_image->filepath, 0, FILEPATH_MAX_LEN);
    strncpy (ppm_image->filepath, filepath, strlen(filepath) + 1);
    //strcpy(ppm_image->filepath, filepath);
    ppm_image->filepath[strlen(filepath)] = END_CHAR;
    return PPM_OK;
}

PPM_STATUS_t ppm_destroy_raster (struct ppm_image* ppm_image)
{
    for (int i = 0; i < ppm_image->height; i++)
    {
        free(ppm_image->raster[i]);
    }
    free (ppm_image->raster);
    return PPM_OK;
}

PPM_STATUS_t ppm_init_raster (struct ppm_image* ppm_image)
{
    ppm_image->raster = ppm_create_raster(ppm_image);
    return PPM_OK;
}

struct ppm_pixel** ppm_create_raster (struct ppm_image* ppm_image)
{
    struct ppm_pixel** raster;
    raster = (struct ppm_pixel**)calloc(ppm_image->height, sizeof(struct ppm_pixel*));
    for (int i = 0; i < ppm_image->height; i++)
    {
        raster[i] = (struct ppm_pixel*)calloc(ppm_image->width, sizeof(struct ppm_pixel));
    }
    return raster;
}

PPM_STATUS_t ppm_load_from_buffer (struct ppm_image* ppm_image, char* buffer)
{
    //TODO: check if initialised before
    if (NULL != ppm_image->raster)
    {
        ppm_image->destroy_raster(ppm_image); /*width and height CAN change thus we need to free allocated memory beforehand*/
    }
    _remove_comments(buffer);
    _replace_all_whitespaces(buffer, COMMENT_CHAR);
    _replace_consecutive_with_single_another(buffer, COMMENT_CHAR, WHITESPACE);

    if ('P' != buffer[0])
    {
        return PPM_ERR_PARAM;
    }

    if ('3' == buffer[1])
    {
        /*P3 version = ascii ppm*/
        ppm_image->magic_number = P3;
    } else if ('6' == buffer[1])
    {
        /*P6 version*/
        ppm_image->magic_number = P6;
    } else {
        return PPM_ERR_PARAM;
    }
    int cursor = 3;
    cursor += _get_next(buffer + cursor, &(ppm_image->width)) + 1; /*read width*/
    cursor += _get_next(buffer + cursor, &(ppm_image->height)) + 1; /*read height*/
    cursor += _get_next(buffer + cursor, &(ppm_image->maxval)) + 1; /*read maxval*/
    
    ppm_image->init_raster(ppm_image);

    for (int row = 0; row < ppm_image->height; row++)
    {
        for (int col = 0; col < ppm_image->width; col++)
        {
            cursor += _get_next(buffer + cursor, &(ppm_image->raster[row][col].red)) + 1; /*read red*/
            cursor += _get_next(buffer + cursor, &(ppm_image->raster[row][col].green)) + 1; /*read green*/
            cursor += _get_next(buffer + cursor, &(ppm_image->raster[row][col].blue)) + 1; /*read blue*/
        }
    }
    
    return PPM_OK;
}

PPM_STATUS_t ppm_load_from_file (struct ppm_image* ppm_image)
{
    //TODO: check if initialised before
    if (NULL != ppm_image->raster)
    {
        ppm_image->destroy_raster(ppm_image); /*width and height CAN change thus we need to free allocated memory beforehand*/
    }
    FILE* ppm_file = NULL;
    ppm_file = fopen(ppm_image->filepath, "r");

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
    } else if ('6' == buffer[1])
    {
        /*P6 version*/
        ppm_image->magic_number = P6;
    } else {
        return PPM_ERR_PARAM;
    }

    /*parse (ignore) initial comment*/
    fgets(buffer, LINE_MAX_LEN, ppm_file);
    if (COMMENT_CHAR == buffer[0])
    {
        fgets(buffer, LINE_MAX_LEN, ppm_file);
    }
    _replace_all_whitespaces(buffer, WHITESPACE);
    int cursor = 0;
    cursor += _get_next(buffer + cursor, &(ppm_image->width)) + 1; /*read width*/
    _get_next(buffer + cursor, &(ppm_image->height)); /*read height*/
    fgets(buffer, LINE_MAX_LEN, ppm_file);
    _get_next(buffer, &(ppm_image->maxval)); /*read maxval*/

    ppm_image->init_raster(ppm_image);

    int i = 0;
    int row = 0;
    int col = 0;
    while (fgets(buffer, LINE_MAX_LEN, ppm_file))
    {
        if (ppm_image->width == col)
        {
            col = 0;
            row++;
        }
        switch(i)
        {
            case 0:
            {
                _get_next(buffer, &(ppm_image->raster[row][col].red));
                break;
            }
            case 1:
            {
                _get_next(buffer, &(ppm_image->raster[row][col].green));
                break;
            }
            case 2:
            {
                i = -1;
                _get_next(buffer, &(ppm_image->raster[row][col].blue));
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
    fclose(ppm_file);
    return PPM_OK;
}

PPM_STATUS_t ppm_save_to_buffer (struct ppm_image* ppm_image, char* buffer)
{    
    switch (ppm_image->magic_number)
    {
        case P3:
        {
            buffer[0] = 'P';
            buffer[1] = '3';
            break;
        }
        case P6:
        {
            buffer[0] = 'P';
            buffer[1] = '6';
            break;
        }
        default:
        {
            buffer[0] = 'P';
            buffer[1] = '3';
            break;
        }
    }
    buffer[2] = WHITESPACE;
    buffer[3] = COMMENT_CHAR;
    int cursor = 4;
    for (cursor = 4; cursor < 4 + strlen(ppm_image->initial_comment); cursor++)
    {
        buffer[cursor] = ppm_image->initial_comment[cursor-4];
    }
    buffer[cursor++] = WHITESPACE;
    _insert_int_to_str(buffer+cursor, ppm_image->width);
    cursor += _strlen_int(ppm_image->width);
    buffer[cursor++] = ' '; /*Exceptionally used space instead of new line to comply with gimp format*/
    _insert_int_to_str(buffer+cursor, ppm_image->height);
    cursor += _strlen_int(ppm_image->height);
    buffer[cursor++] = WHITESPACE;
    _insert_int_to_str(buffer+cursor, ppm_image->maxval);
    cursor += _strlen_int(ppm_image->maxval);

    for (int row = 0; row < ppm_image->height; row++)
    {
        for (int col = 0; col < ppm_image->width; col++)
        {
            buffer[cursor++] = WHITESPACE;
            _insert_int_to_str(buffer+cursor, ppm_image->raster[row][col].red);
            cursor += _strlen_int(ppm_image->raster[row][col].red);

            buffer[cursor++] = WHITESPACE;
            _insert_int_to_str(buffer+cursor, ppm_image->raster[row][col].green);
            cursor += _strlen_int(ppm_image->raster[row][col].green);

            buffer[cursor++] = WHITESPACE;
            _insert_int_to_str(buffer+cursor, ppm_image->raster[row][col].blue);
            cursor += _strlen_int(ppm_image->raster[row][col].blue);
        }
    }
    buffer[cursor++] = END_CHAR;
    return PPM_OK;
}

PPM_STATUS_t ppm_save_to_file (struct ppm_image* ppm_image)
{
    PPM_STATUS_t retval = PPM_OK;
    FILE* ppm_file = NULL;
    ppm_file = fopen(ppm_image->filepath, "w");
    if (NULL == ppm_file)
    {
        printf("Error: cannot create file: %s\n", ppm_image->filepath);
        return PPM_ERR_IO;
    }

    char buffer[LINE_MAX_LEN];
    memset (buffer, 0, LINE_MAX_LEN);
    switch (ppm_image->magic_number)
    {
        case P3:
        {
            buffer[0] = 'P';
            buffer[1] = '3';
            break;
        }
        case P6:
        {
            buffer[0] = 'P';
            buffer[1] = '6';
            break;
        }
        default:
        {
            buffer[0] = 'P';
            buffer[1] = '3';
            break;
        }
    }
    buffer[2] = WHITESPACE;
    fputs(buffer, ppm_file);
    memset (buffer, 0, LINE_MAX_LEN);
    buffer[0] = COMMENT_CHAR;
    int cursor = 1;
    for (cursor = 1; cursor < 1 + strlen(ppm_image->initial_comment); cursor++)
    {
        buffer[cursor] = ppm_image->initial_comment[cursor-1];
    }
    buffer[cursor++] = WHITESPACE;
    fputs(buffer, ppm_file);
    memset (buffer, 0, LINE_MAX_LEN);
    cursor = 0;
     _insert_int_to_str(buffer+cursor, ppm_image->width);
    cursor += _strlen_int(ppm_image->width);
    buffer[cursor++] = ' '; /*Exceptionally used space instead of new line to comply with gimp format*/
    _insert_int_to_str(buffer+cursor, ppm_image->height);
    cursor += _strlen_int(ppm_image->height);
    buffer[cursor++] = WHITESPACE;
    _insert_int_to_str(buffer+cursor, ppm_image->maxval);
    cursor += _strlen_int(ppm_image->maxval);
    buffer[cursor++] = WHITESPACE;
    fputs(buffer, ppm_file);

    for (int row = 0; row < ppm_image->height; row++)
    {
        for (int col = 0; col < ppm_image->width; col++)
        {
            memset (buffer, 0, LINE_MAX_LEN);
            cursor = 0;
            _insert_int_to_str(buffer+cursor, ppm_image->raster[row][col].red);
            cursor += _strlen_int(ppm_image->raster[row][col].red);
            buffer[cursor++] = WHITESPACE;

            _insert_int_to_str(buffer+cursor, ppm_image->raster[row][col].green);
            cursor += _strlen_int(ppm_image->raster[row][col].green);
            buffer[cursor++] = WHITESPACE;

            _insert_int_to_str(buffer+cursor, ppm_image->raster[row][col].blue);
            cursor += _strlen_int(ppm_image->raster[row][col].blue);
            buffer[cursor++] = WHITESPACE;

            fputs(buffer, ppm_file);
        }
    }
    memset (buffer, 0, LINE_MAX_LEN);
    buffer[0] = END_CHAR;
    fputs(buffer, ppm_file);

    fclose(ppm_file);
    return retval;
}


PPM_STATUS_t ppm_set_pixel(struct ppm_image* ppm_image, const int row, const int col, const uint16_t red, const uint16_t green, const uint16_t blue)
{
    ppm_image->raster[row][col].red = red;
    ppm_image->raster[row][col].green = green;
    ppm_image->raster[row][col].blue = blue;
    return PPM_OK;
}
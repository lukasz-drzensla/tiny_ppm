/* 
* Copyright (C) 2023-2024  Lukasz Drzensla
*/

#ifndef _HLP_PPM_H
#define _HLP_PPM_H

#define _HEADER_LEN  MAGIC_NUMBER_LEN + COMMENT_LINE_LEN + 3 * UINT16_STR_LEN + 5 * WHITESPACE_LEN
#define _RASTER_LEN (UINT16_STR_LEN + WHITESPACE_LEN) * 3
#define _BUFFER_LEN(image) _HEADER_LEN + _RASTER_LEN * image->width * image->height
#define WHITESPACE '\n'
#define END_CHAR '\0'
#define COMMENT_CHAR '#'

#ifndef FALSE
#define FALSE 0
#endif /* FALSE */

#ifndef TRUE
#define TRUE 1
#endif /* TRUE */

#endif /* _HLP_PPM_H */

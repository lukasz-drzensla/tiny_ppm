/* 
* Copyright (C) 2023  Lukasz Drzensla
*/

#include "ppm.h"
#include "hlp_ppm.h"

int _power (const int base, const int exp);
int _strlen_int (const int var);
int _insert_int_to_str (char* dest, const int var);
void _remove_comments (char *buffer);
void _replace_all_whitespaces (char* buffer, const char c);
void _replace_consecutive_with_single_another (char* buffer, const char c, const char another);
int _get_next(char* buffer, uint16_t* dest);


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
    char new_buffer[BUF_MAX_LEN];
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

/* 
* Copyright (C) 2023-2024  Lukasz Drzensla
*/

#include "ppm.h"
#include "hlp_ppm.h"

int power (const int base, const int exp);
int strlen_int (const int var);
int insert_int_to_str (char* dest, const int var);
void replace_all_whitespaces (char* buffer, const char c);
int get_next(char* buffer, uint16_t* dest);

int power (const int base, const int exp)
{
    int result = 1;
    for (int i = 0; i < exp; i++)
    {
        result *= base;
    }
    return result;
}

int strlen_int (const int var)
{ 
    int count = 0;
    int _var = var;
    do { 
        _var /= 10; ++count;
    } while (_var != 0);
    return count;
}

int insert_int_to_str (char* dest, const int var)
{
    int len = strlen_int (var);
    for (int i = 0; i < len; i++)
    {
        dest[i] = (char) ((var / power (10, len-i-1) % 10) + 48);
    }
    return len;
}

void replace_all_whitespaces (char* buffer, const char c)
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

/**
*@brief This function interprets the first word in a string as a 16-bit unsigned integer
*@param buffer a string that contains integers represented as digits eg 'test 123 test 456'
*@param dest a pointer to an integer where the result will be stored
*@return The number of characters processed - can be then used in next function call to move forward in the buffer and thus get next integer in a string
*/
int get_next(char* buffer, uint16_t* dest)
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
        temp += (((uint16_t)temp_buf[j]) - 48) * (uint16_t)power(10, i - j - 1);
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

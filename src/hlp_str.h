/* 
* Copyright (C) 2023  Lukasz Drzensla
*/

#ifndef _HLP_STR_H
#define _HLP_STR_H

int power (const int base, const int exp);
int strlen_int (const int var);
int insert_int_to_str (char* dest, const int var);
void replace_all_whitespaces (char* buffer, const char c);
int get_next(char* buffer, uint16_t* dest);

#endif /* _HLP_STR_H */

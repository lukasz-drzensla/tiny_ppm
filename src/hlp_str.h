#ifndef _HLP_STR_H
#define _HLP_STR_H

int _power (const int base, const int exp);
int _strlen_int (const int var);
int _insert_int_to_str (char* dest, const int var);
void _remove_comments (char *buffer);
void _replace_all_whitespaces (char* buffer, const char c);
void _replace_consecutive_with_single_another (char* buffer, const char c, const char another);
int _get_next(char* buffer, uint16_t* dest);

#endif /* _HLP_STR_H */

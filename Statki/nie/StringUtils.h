#ifndef STRING_UTILS
#define STRING_UTILS

char* concat(const char* string1, const char* string2);
char* concat(const char* string, const char ch);
char* concat(const char ch, const char* string);
void copy(const char* from, char* to);
int strLength(const char* string);

#endif

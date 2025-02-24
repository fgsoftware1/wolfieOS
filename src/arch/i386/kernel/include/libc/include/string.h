#ifndef STRING_H
#define STRING_H

#include "types.h"

void *memset(void *dst, char c, u32 n);
void *memcpy(void *dst, const void *src, u32 n);
int memcmp(u8 *s1, u8 *s2, u32 n);

int strlen(const char *s);
int strcmp(const char *s1, char *s2);
int strncmp(const char *s1, const char *s2, int c);
int strcpy(char *dst, const char *src);
void strcat(char *dest, const char *src);
char *strstr(const char *in, const char *str);

int isspace(char c);
int isalpha(char c);
char upper(char c);
char lower(char c);

void itoa(char *buf, int base, int d);
void ftoa(char *buf, float f);

void float_print(const char *msg, float f, const char *end) ;

#endif

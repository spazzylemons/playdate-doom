#ifndef __DOOM_CONFIG_H__
#define __DOOM_CONFIG_H__


#if defined(WIN32)
#define DOOM_WIN32
#elif defined(__APPLE__)
#define DOOM_APPLE
#else
#define DOOM_LINUX
#endif


#include "DOOM.h"


#define doom_abs(x) ((x) < 0 ? -(x) : (x))


extern char error_buf[260];
extern int doom_flags;
void doom_print(const char *str);
void *doom_malloc(int size);
void doom_free(void *ptr);
void *doom_open(const char *filename, const char *mode);
void doom_close(void *handle);
int doom_read(void *handle, void *buf, int count);
int doom_write(void *handle, const void *buf, int count);
int doom_seek(void *handle, int offset, doom_seek_t origin);
int doom_tell(void *handle);
int doom_eof(void *handle);
void doom_gettime(int *sec, int *usec);
void doom_exit(int code);
char *doom_getenv(const char *var);


const char* doom_itoa(int i, int radix);
const char* doom_ctoa(char c);
const char* doom_ptoa(void* p);
void doom_memset(void* ptr, int value, int num);
void* doom_memcpy(void* destination, const void* source, int num);
int doom_fprint(void* handle, const char* str);
int doom_strlen(const char* str);
char* doom_concat(char* dst, const char* src);
char* doom_strcpy(char* destination, const char* source);
char* doom_strncpy(char* destination, const char* source, int num);
int doom_strcmp(const char* str1, const char* str2);
int doom_strncmp(const char* str1, const char* str2, int n);
int doom_strcasecmp(const char* str1, const char* str2);
int doom_strncasecmp(const char* str1, const char* str2, int n);
int doom_atoi(const char* str);
int doom_atox(const char* str);
int doom_toupper(int c);


#endif

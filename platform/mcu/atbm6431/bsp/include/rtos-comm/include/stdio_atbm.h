#ifndef __STDIO_H___ATBM_
#define __STDIO_H___ATBM_
#include "type.h"
#include <stdio.h>
#include <stdarg.h>	/* va_list, va_arg() */
#include <stddef.h>	/* size_t */
#include <stdlib.h>
#include <sys/reent.h>
#include <stdbool.h>
#if 0
//#include <dirent.h>
// FIXME: find a right place to define the following things
// constants
typedef int fpos_t;
#define FSEEK_SET   0   // origin is beginning of the file
#define FSEEK_CUR   1   // origin is current position
#define FSEEK_END   2   // origin is end of the file

//#define FILE struct __FILE


#ifndef __time_t_defined
typedef long time_t;
#define __time_t_defined
#endif //__time_t_defined
#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED
struct timeval {
  long      tv_sec;
  long 		tv_usec;
};
#endif //_TIMEVAL_DEFINED
#if 0
struct stat {
        int             st_dev;     /* ID of device containing file */
	int             st_ino;     /* inode number */
	int             st_mode;    /* protection */
	int             st_size;    /* total size, in bytes */
	int             st_blksize; /* blocksize for file system I/O */
	int             st_blocks;  /* number of 512B blocks allocated */
	time_t          st_atime;   /* time of last access */
	time_t          st_mtime;   /* time of last modification */
	time_t          st_ctime;   /* time of last status change */
};
#endif //

//#define stdin	((void*)0)
//#define stdout	((void*)0)
//#define stderr	((void*)0)

#define	stdin	(_REENT->_stdin)
#define	stdout	(_REENT->_stdout)
#define	stderr	(_REENT->_stderr)
#define EOF	(-1)

#define EOF		(-1)

int vsprintf(char *buffer, const char *fmt, va_list args);
int sprintf(char *buffer, const char *fmt, ...);
int vprintf(const char *fmt, va_list args);
int printf(const char *fmt, ...);
//void printf_task();
extern int snprintf (char *, size_t, const char *, ...);
extern int vsnprintf(char *str, size_t size, const char *fmt, va_list ap) ;

#if 0
int fprintf(FILE *stream, const char *format, ...);
int fgetc(FILE *stream);
char *fgets(char *s, int size, FILE *stream);
int getc(FILE *stream);
int getchar(void);
char *gets(char *s);
int ungetc(int c, FILE *stream);

int fputc(int c, FILE *stream);
int fputs(const char *s, FILE *stream);
int putc(int c, FILE *stream);
int putchar(int c);
int puts(const char *s);
FILE *fopen(const char *path, const char *mode);
FILE *fdopen(int fd, const char *mode);
FILE *freopen(const char *path, const char *mode, FILE *stream);
int fclose(FILE *fp);
int fread(void *ptr, int size, int nmemb, FILE *stream);
int fwrite(const void *ptr, int size, int nmemb, FILE *stream);
struct dir *opendir(const char *path);
struct dirent *readdir(struct dir *dirp);
int closedir(struct dir *dirp);
int fstat(FILE* fd, struct stat *buf);
#endif//0
void fflush(void *stream);
#endif //0
#endif /* __STDIO_H__ */

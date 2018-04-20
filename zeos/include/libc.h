/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>

int write(int fd, char *buffer, int size);
int gettime();
int getpid();

void itoa(int a, char *b);

int strlen(char *a);

int getpid();

int fork();

int yield();

void exit();

void perror();

/*pid: process identifier whose statistics get_stats will get
st: pointer to the user buffer to store the statistics*/
int get_stats(int pid, struct stats *st);


#endif  /* __LIBC_H__ */

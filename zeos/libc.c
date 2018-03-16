/*
 * libc.c 
 */

#include <libc.h>
#include <errno.h>
#include <types.h>

int errno = 0;

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

void perror() {
	if (errno == -1) write(1,"Error\n", 6);
	if (errno == -EBADF) write(1,"Bad file number\n",17);
	if (errno == -EACCESS) write(1,"Permission denied\n",19);
	if (errno == -ENOSYS) write(1,"Function not implemented\n",26);
}


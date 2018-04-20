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
  switch(errno) {
    char *textBuffer;
    case -EBADF:
      write(1,"Bad file number\n",17);
      break;
    case -EACCESS:
      write(1,"Permission denied\n",19);
      break;
    case -ENOSYS:
      write(1,"Function not implemented\n",26);
      break;
    case -EAGAIN:
      textBuffer = "Error: Try again  \n";
      write(1, textBuffer, strlen(textBuffer));
      break;
    case -ENOMEM:
      textBuffer = "Error: Out of memory  \n";
      write(1, textBuffer, strlen(textBuffer));
      break;
    default:
      write(1,"Error\n", 6);
  }
}


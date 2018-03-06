/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

int sys_write(int fd, char *buffer, int size) {
	int checked = check_fd(fd, ESCRIPTURA);
	if (checked < 0) return checked;
	if (buffer == NULL) return -1;
	if (size < 0) return -1;
	
	char *kernel;
	copy_from_user(kernel,buffer, size);

	int wconsole = sys_write_console (kernel, size);
	return wconsole;
}

void sys_exit()
{  
}

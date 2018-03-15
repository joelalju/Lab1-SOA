/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>
#include <system.h>

#include <sched.h>

#define LECTURA 0
#define ESCRIPTURA 1

extern zeos_ticks;

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

#define BUFFER_SIZE 512

int sys_write(int fd, char *buffer, int size) {
	//Error control
	int checked = check_fd(fd, ESCRIPTURA);
	if (checked < 0) {
		return checked;
	}
	if (buffer == NULL) {
		return -1;
	}
	if (size < 0) {
		return -1;
	}

	//Buffer print
	char kernelBuffer[BUFFER_SIZE];
	int bytesToPrint = size;
	int bytesPrinted;
	while (bytesToPrint > BUFFER_SIZE) {
		copy_from_user(buffer, kernelBuffer, BUFFER_SIZE);
		bytesPrinted = sys_write_console(kernelBuffer, BUFFER_SIZE);
		bytesToPrint -= bytesPrinted;
		buffer += bytesPrinted;
	}
	//Print remaining bytes
	if (bytesToPrint > 0) {
		copy_from_user(buffer, kernelBuffer, bytesToPrint);
		bytesPrinted = sys_write_console(kernelBuffer, bytesToPrint);
		bytesToPrint -= bytesPrinted;
	}

	return (size - bytesToPrint);
}

int sys_gettime() {
	return zeos_ticks;
}

void sys_exit()
{  
}

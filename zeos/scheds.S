#include <asm.h>
#include <segment.h>
#include <errno.h>

ENTRY(task_switch)
	pushl %esi;
	pushl %edi;
	pushl %ebx;
	movl 4(%esp), %ebx;
	call inner_task_switch;
	popl %ebx;
	popl %edi;
	popl %esi;
	ret;
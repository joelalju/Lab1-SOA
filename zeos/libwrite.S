#include <asm.h>
#include <segment.h>
/** #include <errno.h>

/**   0(%esp) - %ebx    \                        **/
/**   4(%esp) - %ecx     |                       **/
/**   8(%esp) - %edx     |                       **/

ENTRY(write)
	movl 0(%esp), %ebx;
	movl 4(%esp), %ecx;
	movl 8(%esp), %edx;
	movl 12(%esp), %esi;
	movl 16(%esp), %edi;
	movl $4, %eax;
	int $0x80;
	cmpl $0, %eax;
	jl err;
	jmp finito;
err:
	/** movl $-ENOSYS, %eax; **/
finito:
	ret;

#include <asm.h>
#include <segment.h>
#include <errno.h>

/**   0(%esp) - %ebx     |                       **/
/**   4(%esp) - %ecx     |                       **/
/**   8(%esp) - %edx     |                       **/

ENTRY(write)
	movl $1, %edx;
	movl $2, %ecx;
	movl $3, %ebx;
	movl $4, %eax;
	int $0x80;
	cmpl $0, %eax;
	jl err;
	jmp finito;
err:
	movl $-ENOSYS, %eax;
finito:
	ret;

ENTRY(clock)
	movl 0(%esp), %ebx;
	movl 4(%esp), %ecx;
	movl 8(%esp), %edx;
	movl 12(%esp), %esi;
	movl 16(%esp), %edi;
	movl $10, %eax;
	int $0x80;
	cmpl $0, %eax;
	jl err2;
	jmp finito2;
err2:
	movl $-ENOSYS, %eax;
finito2:
	ret;

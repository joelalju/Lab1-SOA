#include <asm.h>
#include <segment.h>
#include <errno.h>

/**   0(%esp) - %ebx     |                       **/
/**   4(%esp) - %ecx     |                       **/
/**   8(%esp) - %edx     |                       **/

ENTRY(write)
	movl 4(%esp), %ebx;
	movl 8(%esp), %ecx;
	movl 12(%esp), %edx;
	movl $4, %eax;
	int $0x80;
	cmpl $0, %eax;
	jl errwrite;
	jmp finwrite;
errwrite:
	movl $-ENOSYS, %eax;
finwrite:
	ret;

ENTRY(gettime)
	movl $10, %eax;
	int $0x80;
	cmpl $0, %eax;
	jl errtime;
	jmp fintime;
errtime:
	movl $-ENOSYS, %eax;
fintime:
	ret;

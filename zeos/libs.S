#include <asm.h>
#include <segment.h>
#include <errno.h>

/**   0(%esp) - %ebx     |                       **/
/**   4(%esp) - %ecx     |                       **/
/**   8(%esp) - %edx     |                       **/

ENTRY(write)
	pushl %ebp;
	movl %esp, %ebp;
	pushl %ebx;
	movl 8(%ebp), %ebx;
	movl 12(%ebp), %ecx;
	movl 16(%ebp), %edx;
	movl $4, %eax;
	int $0x80;
	popl %ebx;
	cmpl $0, %eax;
	jl errwrite;
	jmp finwrite;
errwrite:
	movl $0, %ecx;
	subl %ecx, %eax;
	movl %eax, errno;
	movl $-1, %eax;
finwrite:
	popl %ebp;
	ret;

ENTRY(gettime)
	pushl %ebp;
	movl %esp, %ebp;
	movl $10, %eax;
	int $0x80;
	cmpl $0, %eax;
	jl errtime;
	jmp fintime;
errtime:
	movl $0, %ecx;
	subl %ecx, %eax;
	movl %eax, errno;
	movl $-1, %eax;
fintime:
	popl %ebp;
	ret;

ENTRY(getpid)
	pushl %ebp;
	movl %esp, %ebp;
	movl $20, %eax;
	int $0x80;
	popl %ebp;
	ret;

ENTRY(fork)
	pushl %ebp;
	movl %esp, %ebp;
	movl $2, %eax;
	int $0x80;
	cmpl $0, %eax;
	jl errtime;
	jmp fintime;
errfork:
	movl $0, %ecx;
	subl %ecx, %eax;
	movl %eax, errno;
	movl $-1, %eax;
finfork:
	popl %ebp;
	ret;

ENTRY(yield)
	pushl %ebp;
	movl %esp, %ebp;
	movl $13, %eax;
	int $0x80;
	popl %ebp;
	ret;

ENTRY(exit)
	pushl %ebp;
	movl %esp, %ebp;
	movl $1, %eax;
	int $0x80;
	popl %ebp;
	ret;

ENTRY(get_stats)
	movl 4(%esp), %ebx;
	movl 8(%esp), %ecx;
	movl $35, %eax;
	int $0x80;
	cmpl $0, %eax;
	jl errstat;
	jmp finstat;
errstat:
	movl $0, %ecx;
	subl %ecx, %eax;
	movl %eax, errno;
	movl $-1, %eax;
finstat:
	ret;

#include <asm.h>
#include <segment.h>
#include <errno.h>

ENTRY(task_switch)
    pushl %ebp;
    movl %esp, %ebp;
    pushl %esi;
    pushl %edi;
    pushl %ebx;
    pushl 8(%ebp);
    call inner_task_switch;
    addl $4, %esp;
    popl %ebx;
    popl %edi;
    popl %esi;
    popl %ebp;
    ret;

ENTRY(change_context)
    mov 4(%esp), %eax;
    mov %ebp, (%eax);
    mov 8(%esp), %esp;
    popl %ebp;
    ret;


ENTRY(get_ebp)
	movl %ebp, %eax;
	ret;

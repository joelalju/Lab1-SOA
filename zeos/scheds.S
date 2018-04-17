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

	/**pushl %esi;**/
	/**pushl %edi;**/
	/**pushl %ebx;**/
	/**movl 4(%esp), %ebx;**/
	/**call inner_task_switch;**/
	/**popl %ebx;**/
	/**popl %edi;**/
	/**popl %esi;**/
	/**ret;**/

ENTRY(get_ebp)
	movl %ebp, %eax;
	ret;

ENTRY(change_context)
    movl 4(%esp), %esp;
    popl %ebp;
    ret;
# 1 "scheds.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "scheds.S"
# 1 "include/asm.h" 1
# 2 "scheds.S" 2
# 1 "include/segment.h" 1
# 3 "scheds.S" 2
# 1 "include/errno.h" 1



# 1 "/usr/include/asm-generic/errno-base.h" 1 3 4
# 5 "include/errno.h" 2
# 4 "scheds.S" 2

.globl task_switch; .type task_switch, @function; .align 0; task_switch:
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

.globl change_context; .type change_context, @function; .align 0; change_context:
    mov 4(%esp), %eax;
    mov %ebp, (%eax);
    mov 8(%esp), %esp;
    popl %ebp;
    ret;


.globl get_ebp; .type get_ebp, @function; .align 0; get_ebp:
 movl %ebp, %eax;
 ret;

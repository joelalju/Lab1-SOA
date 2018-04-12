/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

//////

struct list_head freequeue;
struct list_head readyqueue;
struct task_struct *idle_task;

//////

#if 1
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif

extern struct list_head blocked;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");
	while(1)
	{
	;
	}
}

void force_task_switch() {
	task_switch((union task_union*)idle_task);
}

void init_idle (void){

	struct list_head *head_task = list_first(&freequeue);
	list_del(head_task);

	struct task_struct *task = list_head_to_task_struct(head_task);
	task->PID = 0;
	allocate_DIR(task);

	//init execution context
	union task_union *union_task = (union task_union*)task;
	//push function address
  	union_task->stack[KERNEL_STACK_SIZE-1] = (unsigned long)&cpu_idle; 
  	//push ebp
  	union_task->stack[KERNEL_STACK_SIZE-2] = 0; 
  	//point to top of the idle task stack
	union_task->task.esp_register = (unsigned long)&union_task->stack[KERNEL_STACK_SIZE-2];

	idle_task = task;

}

void init_task1(void){
	struct list_head *head_task = list_first(&freequeue);
	list_del(head_task);

	struct task_struct *task = list_head_to_task_struct(head_task);
	task->PID = 1;
	allocate_DIR(task);
	set_user_pages(task);

	//init execution context
	union task_union *union_task = (union task_union*)task;
	tss.esp0 = (DWord)&union_task->stack[KERNEL_STACK_SIZE];
	set_cr3(get_DIR(task));

	//test
	//task_switch(union_task);
}

void init_ready_queue(){
	//init readyqueue
	INIT_LIST_HEAD(&readyqueue);
}

void init_free_queue(){
	//init freequeue
	INIT_LIST_HEAD(&freequeue);
	for(int i = 0; i < NR_TASKS; ++i) {
		list_add_tail(&(task[i].task.list), &freequeue);
	}
}


void init_sched(){
	
	
	
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

void inner_task_switch (union task_union *new) {
	//update tss to make it point to the new_task system stack
	tss.esp0 = (DWord)&new->stack[KERNEL_STACK_SIZE];
	//change user addres space updating the current page directory
	set_cr3(get_DIR(&new->task));
	//store the current EBP register in the PCB
  	new->stack[KERNEL_STACK_SIZE-1] = tss.ebp; 
  	//change the current system stack setting ESP register to point to the stored value in the new PCB
	new->task.esp_register = (unsigned long)&new->stack[KERNEL_STACK_SIZE];
	//restore the EBP register from the stack
	tss.ebp = new->stack[KERNEL_STACK_SIZE-1];
	//return to the routine that called this one using the instruction RET
	return;
}

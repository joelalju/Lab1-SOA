/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <schedperf.h>
#include <mm.h>
#include <io.h>
#include <system.h>
#include <stats.h>

/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

//////

int quantum;

// free task structs
struct list_head freequeue;
// ready queue
struct list_head readyqueue;

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

void force_task_switch(){
  update_process_state_rr(current(), &readyqueue);
  sched_next_rr();
}

void init_idle (void){

	struct list_head *head_task = list_first(&freequeue);
	list_del(head_task);

	struct task_struct *task = list_head_to_task_struct(head_task);
	task->PID = 0;

	task->quantum = 10;
	default_stats(&task->stat_list);

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

void default_stats(struct stats *s)
{
	s->user_ticks = 0;
	s->system_ticks = 0;
	s->blocked_ticks = 0;
	s->ready_ticks = 0;

	s->elapsed_total_ticks = zeos_ticks;
	s->total_trans = 0;
	s->remaining_ticks = zeos_ticks;
}

void init_task1(void){

	struct list_head *head_task = list_first(&freequeue);
	list_del(head_task);

	struct task_struct *task = list_head_to_task_struct(head_task);
	task->PID = 1;
	task->quantum = 10;

	default_stats(&task->stat_list);

	allocate_DIR(task);
	set_user_pages(task);

	//init execution context
	union task_union *union_task = (union task_union*)task;
	tss.esp0 = (DWord)&union_task->stack[KERNEL_STACK_SIZE];
	set_cr3(get_DIR(task));

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
	init_free_queue();
	init_ready_queue();
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

	struct task_struct *task = &new->task;

  	// pdate TSS to make it point to the new_task system stack
  	tss.esp0 = KERNEL_ESP(new);

  	// TLB flush. change user addres space updating the current page directory
    set_cr3(get_DIR(task));

  	change_context(&current()->esp_register, task->esp_register);
}

void update_sched_data_rr() {
	int aux = get_quantum(current());
  set_quantum(current(), aux-1);
}

int needs_sched_rr() {
	if (get_quantum(current()) <= 0) return 1;
	else return 0;
}


//t is the current task, and dst_queue the new state of the current task
void update_process_state_rr (struct task_struct *t,struct list_head *dst_queue) {
  //get_stats(t->PID, &(t->stat_list));
	if (t->PID != current()->PID ) list_del(&(t->list));
	if (dst_queue != NULL) list_add_tail(&(t->list), dst_queue);
}

void sched_next_rr() {
	struct list_head *next_process = list_first(&readyqueue);
  	struct task_struct *np_aux = (struct task_struct*)((int)next_process&0xfffff000);
	list_del(next_process);
  	task_switch((union task_union*)next_process);
}

void schedule() {
  update_sched_data_rr();
  if (needs_sched_rr() && !list_empty(&readyqueue) && current()->PID != 0) {
    set_quantum (current(), 10);
    update_process_state_rr(current(), &readyqueue);
    sched_next_rr();
  }
}

int get_quantum (struct task_struct *t) {
  return t->quantum;

}

void set_quantum (struct task_struct *t, int new_quantum) {
  t->quantum = new_quantum;
}

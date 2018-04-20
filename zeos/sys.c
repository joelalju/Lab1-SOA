/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>
#include <system.h>

#include <sched.h>

#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1

void * get_ebp();

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int global_PID=1000;

int ret_from_fork()
{
  return 0;
}

int sys_fork()
{
  	// creates the child process
  	//get a free task_struct
  	if (list_empty(&freequeue)) {
  		return -ENOMEM;
  	}
  	struct list_head *head_child_task = list_first(&freequeue);
	list_del(head_child_task);
	struct task_struct *child_task = list_head_to_task_struct(head_child_task);

	//copy the parent's task_union to the child
	union task_union *child_union_task = (union task_union*)task;
	copy_data(current(), child_union_task, sizeof(union task_union));

	//initialize field dir_pages_baseAddr with a new directory
	allocate_DIR(child_task);

	//search physical pages in wich to map logical pages for data+stack of child process
	int pag;
	int new_ph_pag;
	page_table_entry *process_PT = get_PT(child_task);
  	/* DATA */
	for (pag = 0; pag < NUM_PAG_DATA; ++pag) {
		new_ph_pag = alloc_frame();
		if (new_ph_pag == -1) {
			//error no pages left
			//reroll lasts allocations
			for (int i = 0; i < pag; ++i) {
				free_frame(get_frame(process_PT, PAG_LOG_INIT_DATA + i));
				del_ss_pag(process_PT, PAG_LOG_INIT_DATA + i);
			}
			//set pcb as free
			list_add_tail(head_child_task, &freequeue);
			return -EAGAIN;
		}
		set_ss_pag(process_PT, PAG_LOG_INIT_DATA + pag, new_ph_pag);
	}
	/* CODE and SYSTEM*/
	page_table_entry *parent_task_PT = get_PT(current());
	for (int i = 0; i < NUM_PAG_KERNEL; ++i) {
		set_ss_pag(process_PT, i, get_frame(parent_task_PT, i));
	}
	for (int i = 0; i < NUM_PAG_CODE; ++i) {
		set_ss_pag(process_PT, PAG_LOG_INIT_CODE + i, get_frame(parent_task_PT, PAG_LOG_INIT_CODE + i));
	}

	// Copy parent's DATA to child. We will use TOTAL_PAGES-1 as a temp logical page to map to
	for (pag=NUM_PAG_KERNEL+NUM_PAG_CODE; pag<NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA; pag++)
	{
		// map one child page to parent's address space.
		set_ss_pag(parent_task_PT, pag+NUM_PAG_DATA, get_frame(process_PT, pag));
		copy_data((void*)(pag<<12), (void*)((pag+NUM_PAG_DATA)<<12), PAGE_SIZE);
		del_ss_pag(parent_task_PT, pag+NUM_PAG_DATA);
	}

	/* Deny access to the child's memory space */
  	set_cr3(get_DIR(current()));

  	child_union_task->task.PID = ++global_PID;
  	//child_union_task->task.state=ST_READY;

	// frame pointer 
  	int register_ebp;	
	// map parent ebp to child stack 
	register_ebp = (int) get_ebp();
	register_ebp = (register_ebp - (int)current()) + (int)(child_union_task);

	child_union_task->task.esp_register = register_ebp + sizeof(DWord);

	DWord temp_ebp = *(DWord*)register_ebp;
	/* prepare child stack for context switch */
	child_union_task->task.esp_register -= sizeof(DWord);
	*(DWord*)(child_union_task->task.esp_register) = (DWord)&ret_from_fork;
	child_union_task->task.esp_register -= sizeof(DWord);
	*(DWord*)(child_union_task->task.esp_register) = temp_ebp;

	/* set stats to 0 */
	//init_stats(&(child_union_task->task.p_stats));

	/* Queue child process into readyqueue */
	//child_union_task->task.state = ST_READY;
	list_add_tail(&(child_union_task->task.list), &readyqueue);

	return child_union_task->task.PID;
}

#define BUFFER_SIZE 512

int sys_write(int fd, char *buffer, int size) {
	//Error control
	int checked = check_fd(fd, ESCRIPTURA);
	if (checked < 0) {
		return checked;
	}
	if (buffer == NULL) {
		return -1;
	}
	if (size < 0) {
		return -1;
	}

	//Buffer print
	char kernelBuffer[BUFFER_SIZE];
	int bytesToPrint = size;
	int bytesPrinted;
	while (bytesToPrint > BUFFER_SIZE) {
		copy_from_user(buffer, kernelBuffer, BUFFER_SIZE);
		bytesPrinted = sys_write_console(kernelBuffer, BUFFER_SIZE);
		bytesToPrint -= bytesPrinted;
		buffer += bytesPrinted;
	}
	//Print remaining bytes
	if (bytesToPrint > 0) {
		copy_from_user(buffer, kernelBuffer, bytesToPrint);
		bytesPrinted = sys_write_console(kernelBuffer, bytesToPrint);
		bytesToPrint -= bytesPrinted;
	}

	return (size - bytesToPrint);
}

int sys_yield()
{
  force_task_switch();
  return 0;
}

int sys_gettime() {
	return zeos_ticks;
}

void sys_exit() {
  struct task_struct *task = current();
  page_table_entry *process = get_PT(task);
  for (int i=0; i<NUM_PAG_DATA; i++){
    free_frame(get_frame(process, PAG_LOG_INIT_DATA+i));
    del_ss_pag(process, PAG_LOG_INIT_DATA+i);
  }

  task->PID=-1;
  list_add_tail(&task->list, &freequeue);
  sched_next_rr();
}

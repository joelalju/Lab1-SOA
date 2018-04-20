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
	force_task_switch();
	return current()->PID;
}

extern struct list_head freequeue;
extern struct list_head readyqueue;

int sys_fork()
{
  int PID=-1;

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
			return -ENOMEM;
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




  return PID;
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

int sys_gettime() {
	return zeos_ticks;
}

void sys_exit() {
  struct task_struct *aux = current();
  page_table_entry *process = get_PT(aux);
  for (int i=0; i<NUM_PAG_DATA; i++){
    free_frame(get_frame(process, PAG_LOG_INIT_DATA+i));
    del_ss_pag(process, PAG_LOG_INIT_DATA+i);
  }

  aux->PID=-1;
  list_add_tail(&aux->list, &freequeue);
  sched_next_rr();
}

int sys_get_stats(int pid, struct stats *st) {
  if (pid < 0) return -1;
  for (int i= 0; i < NR_TASKS; ++i) {
    if (task[i].task.PID == pid) {
       int aux_tick;
       if(task[i].task.PID == current()->PID) aux_tick = task[i].task.stat_list.system_ticks;
       else if(&task[i].task.list == &readyqueue) aux_tick = task[i].task.stat_list.ready_ticks;
       else aux_tick = task[i].task.stat_list.user_ticks;

       int current_ticks = get_ticks();       
       aux_tick += current_ticks - task[i].task.stat_list.elapsed_total_ticks;
       task[i].task.stat_list.elapsed_total_ticks = current_ticks;
       return 0;
    }
  }
  return -1;
}

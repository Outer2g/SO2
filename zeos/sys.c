/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1
extern int ZEOS_TICK;
int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -EACCES; /*EACCES*/
  return 0;
}

//system call 'write' routine
int sys_write( int fd, char* buffer, int size){
  //checking fd
  int ret = check_fd(fd,ESCRIPTURA);
  if (ret != 0) return ret;
  //Checking buffer not null
  if (buffer == NULL){
    return -EFAULT;
  }
  //checking positive size
  if (size <0) return -EINVAL;
  char auxBuffer[30];
  int printed = 0;
  //we copy and print until the buffer cannot fully contain the text
  while(size >= 30){
  	ret = copy_from_user(buffer,auxBuffer,30);
  	if (ret != 0) return ret; //error copying from user
  	printed += sys_write_console(auxBuffer,30);
  	buffer += 30;
  	size -= 30;
  }
  //we print the remainings
  ret = copy_from_user(buffer,auxBuffer,size);
  if (ret != 0 ) return ret; //error copying from user
  printed += sys_write_console(auxBuffer,size);
  return printed;
}
int sys_gettime(){
    return ZEOS_TICK;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int ret_from_fork(){
    return 0;
}
void ret_from_clone(){}

int sys_fork()
{
  int PID=-1;
  // creates the child process
  //get free new task if theres none, return error
  if (list_empty(&freequeue)) return -ENOMEM;
  struct list_head *e = list_first(&freequeue);
  list_del(e);
  union task_union * child = list_entry(e,struct task_struct,list);
  union task_union * parent = current();
  //copy parent data into child, nova taula? 
  copy_data(parent,child,sizeof(union task_union));

  allocate_DIR(&child->task);
  //check if we have enough pages
  int i,j;
  //position in vector is the number of the reserved page 
  int RP[NUM_PAG_DATA];
  for (i = 0; i< NUM_PAG_DATA; i++){
      RP[i] = alloc_frame();
      if (RP[i] == -1){
          //if error occurred, it means there weren't enough space => free the taken space and ret error
          for (j = 0; j < i-1; j++){
              free_frame(RP[j]);
        }
        //process back to freequeue
        list_add_tail(&(child->task.list),&freequeue);
        return -ENOMEM;
    }
  }
  //Inherit user data, getting the process pages
  page_table_entry * child_PT = get_PT(&child->task);
  page_table_entry * parent_PT = get_PT(current());
  
  // child will point to kernel pages and parent code
  for (i=0;i<PAG_LOG_INIT_DATA;i++){
      int frame = get_frame(parent_PT,i);
      set_ss_pag(child_PT,i,frame);
  }
  // e) ii) copy user data+stack pages from parent to child, using tmp page
  int unusedmem = PAG_LOG_INIT_DATA + NUM_PAG_DATA;
  for (i = 0; i < NUM_PAG_DATA; i++)
  {
      // assignem els frames a la part de data de child
    set_ss_pag(child_PT, PAG_LOG_INIT_DATA + i, RP[i]);
    // cal inicialitzar noves entrades de la taula de pagines del pare: en aquest cas , despres de la regio data +stack ja
    set_ss_pag(parent_PT, unusedmem + i,RP[i]);
      // aprofito per fer la copia (la taula de pagines actual segueix sent la del pare)
      // al copiar cal transformar en adreces (eren l'indeX de la TP)
    copy_data((void *) ((PAG_LOG_INIT_DATA + i) << 12), (void *) ((unusedmem + i) << 12), PAGE_SIZE); // Page size son 4KB
    // unlink parent pt -- physycal pages
    del_ss_pag(parent_PT, unusedmem + i);
  }
  //flush tlb
  set_cr3(parent->task.dir_pages_baseAddr);
  //assign the new process a new PID
  int childPID = next_pid();
  child->task.PID = childPID;
  //modify not common fields of the child
  child->task.state = ST_READY;
  //prepare stack for task_switch
  child->stack[KERNEL_STACK_SIZE - 18] = &(ret_from_fork);
  child->stack[KERNEL_STACK_SIZE - 19] = 0;
  child->task.kernel_esp = &child->stack[KERNEL_STACK_SIZE - 19];
  //delete it from freequeue TODO
  //add it to readyqueue
  list_add_tail(&(child->task.list),&readyqueue);


  
  return childPID;
}

int sys_clone(void(*function)(void),void *stack){
	//checking correct address
	if (!access_ok(VERIFY_READ,function, sizeof(void *))) return -EACCES;
	if (!access_ok(VERIFY_READ,stack, sizeof(void *))) return -EACCES;
	if (list_empty(&freequeue)){
		//out of PCBs
		return -5; // nomem
	}
	struct list_head *e = list_first(&freequeue);
	union task_union *thread = list_head_to_task_struct(e);
	list_del(e);
	
	struct task_struct *parent = current();
	
	unsigned int pos = parent->pos_dir;
	++info_dir[pos];
	thread->task.pos_dir = pos;
	
	//copy task_union parent to child
	copy_data(parent,thread,sizeof(union task_union));
	
	thread->task.PID = next_pid();
	update_process_state_rr(thread,&readyqueue);
	
	thread->stack[1022] = stack;
	thread->stack[1019] = function;
	thread->stack[1018] = ret_from_fork;
	thread->task.kernel_esp = &thread->stack[1017];
	
	return thread->task.PID;
}

int next_pid(){
    ++global_pid;
    return global_pid;
}

void sys_exit()
{ 
  struct task_struct * curr = current();
  page_table_entry * currentPT = get_PT(curr);
  /* old 
  int i;
  for(i = 0; i< NUM_PAG_DATA;i++){
    free_frame(get_frame(currentPT,PAG_LOG_INIT_DATA+i));
    del_ss_pag(currentPT,PAG_LOG_INIT_DATA+i);
  }
  curr->state = ST_DEAD;
  //add task to freequeue
  list_add(&curr->list,&freequeue);
  */
  //free only non-shared frames
  int i;
  --info_dir[curr->pos_dir];
  if (info_dir[curr->pos_dir] == 0){
  	//no one pointing at this dir => free the memory
  	for ( i = 0; i < NUM_PAG_DATA;++i){
  		int frame = get_frame(currentPT,PAG_LOG_INIT_DATA + i);
  		free_frame(frame);
  		del_ss_pag(currentPT,PAG_LOG_INIT_DATA + i);
  	}
  }
  curr->state = ST_DEAD;
  list_add(&curr->list,&freequeue);
  //execute next process
  sched_next_rr();
}

int sys_get_stats(int pid,struct stats *st){
  if (!access_ok(VERIFY_WRITE,st,sizeof(struct stats))) return -EFAULT;

  if (pid < 0) return -EINVAL;

  int i;
  for (i=0;i<NR_TASKS;i++){
    if(task[i].task.PID==pid){
      if(task[i].task.state == ST_DEAD) return -ESRCH;
      task[i].task.process_stats.remaining_ticks = ticksExec;
      copy_to_user(&(task[i].task.process_stats),st,sizeof(struct stats));
      return 0;
    }
  }
  return -ESRCH;
}

int sys_sem_init(int n_sem, unsigned int value)
{
    if (n_sem >= NR_SEM || n_sem < 0) return -EINVAL;
    struct semaphore mysem = sem_array[n_sem];
    if (mysem.ownerPID != -1) return -EBUSY;
    //else this semaphore is free
    else{
	    int aux = current()->PID;
	    mysem.ownerPID = aux;
	    sem_array[n_sem].value = value;
	    INIT_LIST_HEAD(&mysem.myblocked);
	    sem_array[n_sem] = mysem;
	}
    return 0;
}

int sys_sem_wait(int n_sem)
{
    if (n_sem >= NR_SEM || n_sem < 0) return -EINVAL;
    struct semaphore mysem = sem_array[n_sem];
    if (mysem.ownerPID == -1) return -EINVAL;
    if(mysem.value > 0){
        mysem.value -= 1;
    }
    else{

        sched_update_queues_state(&mysem.myblocked,current());
        sched_switch_process();
        current()->state = ST_BLOCKED;
    }
    return 0;
}

int sys_sem_signal(int n_sem)
{
    if (n_sem >= NR_SEM || n_sem < 0) return -EINVAL;
    struct semaphore mysem = sem_array[n_sem];
    if (mysem.ownerPID == -1) return -EINVAL;
    if(list_empty(&mysem.myblocked) == 1){
        mysem.value += 1;
    }
    else{
        struct list_head *e = list_first(&mysem.myblocked);
        struct task_struct *t = list_head_to_task_struct(e);
        t->state=ST_READY;
        sched_update_queues_state(&readyqueue,t);
        list_del(e);
    }
    return 0;
}

int sys_sem_destroy(int n_sem)
{
    if (n_sem >= NR_SEM || n_sem < 0) return -EINVAL;
    struct semaphore mysem = sem_array[n_sem];
    if (mysem.ownerPID == -1) return -EINVAL;
    if(mysem.ownerPID != current()->PID) return -EPERM;
    while(list_empty(&mysem.myblocked) == 0){
        struct list_head *e = list_first(&mysem.myblocked);
        struct task_struct *t = list_head_to_task_struct(e);
        sched_update_queues_state(&readyqueue,t);
        t->state=ST_READY;
        list_del(e);
    }
    mysem.ownerPID = -1;
    return 0;
}

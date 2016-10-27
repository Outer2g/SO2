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

int sys_fork()
{
  int PID=-1;
  // creates the child process
  //get free new task if theres none, return error
  struct list_head *e = list_first(&freequeue);
  
    if (list_empty(e)) return -ENOMEM;
  union task_union * child = list_entry(e,struct task_struct,list);
  
  union task_union * parent = current();
  //copy parent data into child, nova taula? 
  copy_data(parent,child,KERNEL_STACK_SIZE);
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
        return -ENOMEM;
    }
  }
  //Inherit user data, getting the process pages
  page_table_entry * child_PT = get_PT(&child->task);
  page_table_entry * parent_PT = get_PT(current());
  
  // child will point to kernel pages and parent code
  for (i=0;i<NUM_PAG_KERNEL + NUM_PAG_CODE;i++){
      int frame = get_frame(parent_PT,i);
      set_ss_pag(child_PT,i,frame);
  }
  // e) ii) copy user data+stack pages from parent to child, using tmp page
  int finalParent = NUM_PAG_CODE + NUM_PAG_DATA + NUM_PAG_KERNEL + 1;
  for (i = 0; i < NUM_PAG_DATA; i++){
      set_ss_pag(child_PT,NUM_PAG_KERNEL+NUM_PAG_CODE + i,RP[i]);
      //tmp page
      set_ss_pag(parent_PT, finalParent + i, RP[i]);
      copy_data((void *)((NUM_PAG_KERNEL + NUM_PAG_CODE + i) * PAGE_SIZE),(void*)((finalParent + i)* PAGE_SIZE),PAGE_SIZE);
      //del tmp page after completing the copy
      del_ss_pag(parent_PT,finalParent +i);
  }
  //assign the new process a new PID
  int childPID = next_pid();
  child->task.PID = childPID;
  //modify not common fields of the child
  //prepare stack for task_switch
  
  //delete it from freequeue
  list_del(e);
  //add it to readyqueue
  list_add(&child->task.list,&readyqueue);


  
  return childPID;
}
int ret_from_fork(){
    return 0;
}
int next_pid(){
    ++global_pid;
    return global_pid;
}

void sys_exit()
{  
}

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
  //get free new task if theres none, return error TODO
  struct list_head *e = list_first(&freequeue);
  union task_union * child = list_entry(e,struct task_struct,list);
  //delete it from freequeue
  list_del(e);
  //add it to readyqueue
  list_add(&child->task.list,&readyqueue);
  union task_union * parent = current();
  //copy parent data into child, nova taula? TODO
  copy_data(parent,child,KERNEL_STACK_SIZE);
  allocate_DIR(&child->task);
  //check if we have enough pages
  int allocated = 0;
  int[NUM_PAG_CODE] frames;
  for (int i = 0; i < NUM_PAG_CODE;++i){
      new_ph_pag = allocate_frame();
      if (new_ph_pag == -1) break;}
  if (allocated < NUM_PAG_CODE) {
      //dellocate
      for (int i = 0; i<allocated;++i){
          free_frame(frames[i]);
    }
}
  
  return PID;
}

void sys_exit()
{  
}

/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#define LECTURA 0
#define ESCRIPTURA 1
extern int ZEOS_TICK;
int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

//system call 'write' routine
int sys_write( int fd, char* buffer, int size){
  //checking fd
  int ret = check_fd(fd,ESCRIPTURA);
  if (ret != 0) return ret;
  //Checking buffer not null
  if (buffer == NULL || buffer[0] == '\0'){
    return -1;
  }
  //checking positive size
  if (size <0) return -1;
  char auxBuffer[30];
  int printed = 0;
  //we copy and print until the buffer cannot fully contain the text
  while(size >= 30){
  	ret = copy_from_user(buffer,auxBuffer,30);
  	if (ret != 0 ) return -1; //error copying from user
  	printed += sys_write_console(auxBuffer,30);
  	buffer += 30;
  	size -= 30;
  }
  //we print the remainings
  ret = copy_from_user(buffer,auxBuffer,size);
  if (ret != 0 ) return -1; //error copying from user
  printed += sys_write_console(auxBuffer,size);
  return ret;
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
  
  return PID;
}

void sys_exit()
{  
}

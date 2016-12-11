/*
 * libc.c 
 */

#include <libc.h>

#include <types.h>

#include <errno.h>
int errno;

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}
//fork system wrapper
int fork(void){
    int pid;
    __asm__ volatile(
    "movl $2,%%eax;"
    "int $0x80;"
    : "=a" (pid) // return result of eax to variable ret
);
    if (pid<0){
      errno = -pid;
      return -1;
    }
    return pid;
}
//exit system wrapper
void exit(void){
    __asm__ volatile(
    "movl $1,%eax;"
    "int $0x80"
);
}
//get_stats system wrapper
int get_stats(int pid,struct stats *st){
    int ret;
    __asm__ volatile(
    "movl $35,%%eax;"
    "int $0x80;"
    : "=a" (ret), // return result of eax to variable ret
      "+b" (pid),
      "+c" (st)
);
    if (ret <0){errno = -ret;return-1;}
    return ret;
}
//getpid system wrapper
int getpid(void){
    int pid;
    __asm__ volatile(
    "movl $20,%%eax;"
    "int $0x80;"
    : "=a" (pid) // return result of eax to variable ret
);
    return pid;
}
//write system wrapper
int write(int fd,char* buffer,int size){
  int ret = -1;
  /*first parameter to %ebx, second to %ecx, third to %edx etc. Then %eax has to be the code for the interruption
  in this case, 4. Then we call the interruption 0x80 and we process the result*/

  __asm__ volatile(
    "movl $4,%%eax;"
    "int $0x80;"
    : "=a" (ret), // return result of eax to variable ret
      "+b" (fd), // copy fd, in ebx
      "+c" (buffer),// copy buffer, in ecx
      "+d" (size) // copy size, in edx
      
);
  if (ret >= 0) return ret;
  else{ errno = -ret;return -1;}
}

//read system wrapper
int read(int fd,char* buffer,int count){
  int ret = -1;
  /*first parameter to %ebx, second to %ecx, third to %edx etc. Then %eax has to be the code for the interruption
  in this case, 3 Then we call the interruption 0x80 and we process the result*/

  __asm__ volatile(
    "movl $3,%%eax;"
    "int $0x80;"
    : "=a" (ret), // return result of eax to variable ret
      "+b" (fd), // copy fd, in ebx
      "+c" (buffer),// copy buffer, in ecx
      "+d" (count) // copy size, in edx
      
);
  if (ret >= 0) return ret;
  else{ errno = -ret;return -1;}
}
int clone(void(*function)(void), void *stack){
	//function = starting address of the function to be executed
	//stack = starting address of a memory region to be used as stack
	int ret;
	__asm__ volatile(
    "movl $19,%%eax;"
    "int $0x80;"
    : "=a" (ret), // return result of eax to variable ret
      "+b" (function), // copy fd, in ebx
      "+c" (stack)// copy buffer, in ecx
      
);
	if (ret >= 0) return ret;
	else {errno = -ret; return -1;}
}
int gettime(){
    int ret = -1;
    __asm__ volatile(
        "movl $10,%%eax;"
        "int $0x80;"
        : "=a" (ret) // return result of eax to variable ret
    );
    return ret;
}
int sem_init(int n_sem,unsigned int value){
  int ret;
  __asm__ volatile(
    "movl $21,%%eax;"
    "int $0x80;"
    : "=a" (ret), // return result of eax to variable ret
      "+b" (n_sem), // copy fd, in ebx
      "+c" (value)// copy buffer, in ecx
      
);
  if (ret >= 0) return ret;
  else {errno = -ret; return -1;}
}
int sem_wait(int n_sem){
  int ret;
  __asm__ volatile(
    "movl $22,%%eax;"
    "int $0x80;"
    : "=a" (ret), // return result of eax to variable ret
      "+b" (n_sem) // copy fd, in ebx
      
);
  if (ret >= 0) return ret;
  else {errno = -ret; return -1;}
}
int sem_signal(int n_sem){
int ret;
  __asm__ volatile(
    "movl $23,%%eax;"
    "int $0x80;"
    : "=a" (ret), // return result of eax to variable ret
      "+b" (n_sem) // copy fd, in ebx
      
);
  if (ret >= 0) return ret;
  else {errno = -ret; return -1;}
  }
int sem_destroy(int n_sem){
  int ret;
  __asm__ volatile(
    "movl $24,%%eax;"
    "int $0x80;"
    : "=a" (ret), // return result of eax to variable ret
      "+b" (n_sem) // copy fd, in ebx
      
);
  if (ret >= 0) return ret;
  else {errno = -ret; return -1;}
}
void perror(const char *str){
    
}

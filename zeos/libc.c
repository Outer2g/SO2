/*
 * libc.c 
 */

#include <libc.h>

#include <types.h>

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

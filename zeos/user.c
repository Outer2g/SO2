#include <libc.h>

char buff[24];

int pid;

long inner(long n){
    int i;
    long suma;
    suma = 0;
    for (i=0;i<n;i++) suma = suma+i;
    return suma;
}
long outer(long n){
    int i;
    long acum;
    acum = 0;
    for(i=0;i<n;i++) acum=acum+inner(i);
    return acum;
}
int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
    /*int pid = fork();
    char buffer[32] = " PID = ";
    write(1,buffer,strlen(buffer));
    int x = getpid();
    if (x == 1) exit();
    char *aux[32];
    itoa(x,aux);
    write(1,aux,strlen(aux));
    write(1,"\n",1);*/
    runjp_rank(4,9);
    //runjp();
    //sbrk(10);
    while(1){
	
	//write(1,c,3);
	}
    return 0;
}

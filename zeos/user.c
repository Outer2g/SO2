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

    /*char buffer[32] = "hello mate\n";
    write(1,buffer,strlen(buffer));
    int i = 0;
    while(1){
        char *aux[30];
        int x = gettime();
        itoa(x,aux);
        write(1,aux,strlen(aux));
        write(1,"\n",1);
    }*/
    while(1);
    return 0;
}

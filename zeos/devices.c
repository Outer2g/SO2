#include <io.h>
#include <utils.h>
#include <list.h>
#include <sched.h>
#include <cbuff.h>
// Queue for blocked processes in I/O 
struct list_head blocked;

int sys_write_console(char *buffer,int size)
{
  int i;
  
  for (i=0; i<size; i++)
    printc(buffer[i]);
  
  return size;
}

int sys_read_keyboard(char *buffer,int size){
	current()->mykb.left = size;
	current()->mykb.pos = 0;
	if(!list_empty(&kqueue)){
		list_add_tail(&(current()->list), &kqueue);
     		current()->state = ST_BLOCKED;
     		sched_next_rr();
	}
	else{
		if(cb_count(&kbuff) >= size){
			for(int i = 0; i < size;++i){
				char aux;
				cb_Read(&kbuff,aux);
				buffer[i] = aux;
			}
		}
		else{
			 if (cb_count(&kbuff) == CBUFF_SIZE){
				for(int i = 0; i < CBUFF_SIZE;++i){
				char aux;
				cb_Read(&kbuff,aux);
				buffer[i] = aux;
				}
				current()->mykb.left = size-CBUFF_SIZE;
				current()->mykb.pos = CBUFF_SIZE;
				list_add_tail(&(current()->list), &kqueue);
	     			current()->state = ST_BLOCKED;
	     			sched_next_rr();
			}
			else{
				list_add_tail(&(current()->list), &kqueue);
	     			current()->state = ST_BLOCKED;
	     			sched_next_rr();
				while(1){
					int n;
					if (cb_count(&kbuff) == CBUFF_SIZE) n = CBUFF_SIZE;
					else n = current()->mykb.left;
					for(int i = 0; i < n;++i){
						char aux;
						cb_Read(&kbuff,aux);
						buffer[current()->mykb.pos+i] = aux;
					}
					current()->mykb.pos = current()->mykb.pos + n;
					if(current()->mykb.pos < current()->mykb.left){	
						list_add(&(current()->list), &kqueue);
	     					current()->state = ST_BLOCKED;
	     					sched_next_rr();
					}
					else return size;
					 
				}
			}
		}
	}
	return size;
}

/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */
struct task_struct *idle_task;

#if 0
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif

extern struct list_head blocked;
/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{ 
	;
	}
}

void init_idle (void)
{
    //PID process = 0, kernel mode only, only if there's not another process to execute
    //PID = 0
    struct list_head * e = list_first( &freequeue );
    union task_union * realelement = list_entry( e, struct task_struct, list);
    realelement->task.PID = 0;
    allocate_DIR(&realelement->task);
    idle_task = &realelement->task;
    
    // init stack
    //push &cpu_idle
    int pos = KERNEL_STACK_SIZE - 1;
    realelement->stack[pos] = &cpu_idle;
    //push initial value to ebp (0)
    --pos;
    realelement->stack[pos] = 0;
    realelement->task.kernel_esp = &realelement->stack[pos];
    
    //delete the task from the freequeue
    list_del(e);
    
}

void init_task1(void)
{
    //PID process = 1,user code, code is in user.c
    //PID = 1
    struct list_head * e = list_first( &freequeue );
    union task_union * realelement = list_entry( e, struct task_struct, list);
    realelement->task.PID = 1;
    allocate_DIR(&realelement->task);
    //init user address space
    set_user_pages(&realelement->task);
    tss.esp0 = &realelement->stack[KERNEL_STACK_SIZE];
    //set page directory as the current
    set_cr3(realelement->task.dir_pages_baseAddr);
    //delete task from the freequeue
    list_del(e);
    //add it to readyqueue
    list_add(&realelement->task.list,&readyqueue);
}


void init_sched(){
    init_freequeue();
    //init readyqueue
    INIT_LIST_HEAD(&readyqueue);
    global_pid = 1;
}
void init_freequeue(){
    INIT_LIST_HEAD( &freequeue );
    union task_union* aux = task;
    int size = NR_TASKS;
    int current = 1;
    while (current < size){
        //list_add (anchor,&list)
        struct task_struct *auxTask = &(aux->task);
        list_add(&(auxTask->list),&freequeue);
        ++aux;
        ++current;
    }
}
void inner_task_switch(union task_union* new){
    //update tss
    tss.esp0 = &(new->stack[KERNEL_STACK_SIZE]);
    //update user mem
    set_cr3(new->task.dir_pages_baseAddr);
    //ebp value stored by C, save ebp to kernel_esp of the current task
    struct task_struct* old = current();
    __asm__ __volatile__(
        "movl %%ebp, %0"
        :
        :"m"(old->kernel_esp)
    );
    //change current stack
    __asm__ __volatile__(
        "movl %0,%%esp"
        :
        :"m"(new->task.kernel_esp)
    );

    /*testing
    if (new->task.PID == 0) printk("soc idle");
    else printk("soy el otro");
    */
    //restore ebp and return
    __asm__ __volatile__(
        "popl %ebp;"
        "ret"
    );
    
}
void task_switch(union task_union* new){
    //new pointer to the task_union of the process that will be executed 
    //saving registers esi edi ebx
    	
    asm("pushl %esi;"
        "pushl %edi;"
        "pushl %ebx");
    inner_task_switch(new);
    asm("popl %ebx;"
        "popl %edi;"
        "popl %esi;");
}
struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}


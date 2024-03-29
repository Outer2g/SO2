/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <cbuff.h>
#include <keyboard.h>
#include <types.h>
#include <stats.h>
#include <mm_address.h>

#define NR_TASKS      10
#define NR_SEM  20
#define KERNEL_STACK_SIZE	1024
#define DEFAULT_QUANTUM 10

enum state_t { ST_RUN, ST_READY, ST_BLOCKED,ST_DEAD };

struct kinfo{
	int size;//chars que queden
	int pos;//posició del buffer per el que anem
};

struct task_struct {
  int PID;			/* Process ID. This MUST be the first field of the struct. */
  int quantum;
  struct stats process_stats;
  struct kinfo mykb;
  unsigned long kernel_esp;
  enum state_t state;
  unsigned int pos_dir;
  int heapSize;
  struct list_head list;
  page_table_entry * dir_pages_baseAddr;
};

struct list_head kqueue;
struct c_buffer kbuff;


struct semaphore {
  int ownerPID;
  unsigned int value;
  int toDestroy;
  struct list_head myblocked;
};

union task_union {
  struct task_struct task;
  unsigned long stack[KERNEL_STACK_SIZE];    /* pila de sistema, per procés */
};
int global_pid;
extern union task_union protected_tasks[NR_TASKS+2];
extern union task_union *task; /* Vector de tasques */
extern struct task_struct *idle_task;
extern int info_dir[NR_TASKS];
extern struct semaphore sem_array[NR_SEM];
struct list_head freequeue;
struct list_head readyqueue;
int ticksExec;



#define KERNEL_ESP(t)       	(DWord) &(t)->stack[KERNEL_STACK_SIZE]

#define INITIAL_ESP       	KERNEL_ESP(&task[1])

/* Inicialitza les dades del proces inicial */
void init_task1(void);

void init_idle(void);

void init_sched(void);

void init_stats(struct stats *st);

void init_freequeue(void);

void init_keyboard(void);

struct task_struct * current();

void update_sched_data_rr(void);

void update_stats_user_time(struct stats *st);

void update_stats_system_time(struct stats *st);

void task_switch(union task_union*t);

struct task_struct *list_head_to_task_struct(struct list_head *l);

int allocate_DIR(struct task_struct *t);

page_table_entry * get_PT (struct task_struct *t) ;

page_table_entry * get_DIR (struct task_struct *t) ;

/* Headers for the scheduling policy */
void sched_next_rr();
void update_process_state_rr(struct task_struct *t, struct list_head *dest);
int needs_sched_rr();
void update_sched_data_rr();

#endif  /* __SCHED_H__ */

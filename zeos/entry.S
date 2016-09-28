/*
 * entry.S - Entry point to system mode from user mode
 */

#include <asm.h>
#include <segment.h>


/**************************************************/
/**** Save & Restore ******************************/
/**                                              **/
/** When we change to privilege level 0 (kernel) **/
/** (through an interrupt, a system call, an     **/
/** exception ...) we must save the state of the **/
/** currently running task (save).               **/
/**                                              **/
/** Stack layout in 'systemCall':                **/
/**                                              **/
/**   0(%esp) - %ebx    \                        **/
/**   4(%esp) - %ecx     |                       **/
/**   8(%esp) - %edx     |                       **/
/**   C(%esp) - %esi     | Register saved        **/
/**  10(%esp) - %edi     |  by 'save'            **/
/**  14(%esp) - %ebp     |                       **/
/**  18(%esp) - %eax     |                       **/
/**  1C(%esp) - %ds      |                       **/
/**  20(%esp) - %es      |                       **/
/**  24(%esp) - %fs      |                       **/
/**  28(%esp) - %gs     /                        **/
/**  2C(%esp) - %eip    \                        **/
/**  30(%esp) - %cs      |                       **/
/**  34(%esp) - %eflags  |  Return context saved **/
/**  38(%esp) - %oldesp  |   by the processor.   **/
/**  3C(%esp) - %oldss  /                        **/
/**                                              **/
/**************************************************/


    
#define SAVE_ALL \
      pushl %gs; \
      pushl %fs; \
      pushl %es; \
      pushl %ds; \
      pushl %eax; \
      pushl %ebp; \
      pushl %edi; \
      pushl %esi; \
      pushl %edx; \
      pushl %ecx; \
      pushl %ebx; \
      movl $__KERNEL_DS, %edx;    \
      movl %edx, %ds;           \
      movl %edx, %es

#define RESTORE_ALL2 \
      movl %gs,0x28(%esp); \
      movl %fs,0x24(%esp); \
      movl %es,0x20(%esp); \
      movl %ds,0x1C(%esp); \
      movl %eax,0x18(%esp); \
      movl %ebp,0x14(%esp); \
      movl %edi,0x10(%esp); \
      movl %esi,0x0C(%esp); \
      movl %edx,0x08(%esp); \
      movl %ecx,0x04(%esp); \
      movl %ebx,(%esp)
#define RESTORE_ALL \
      popl %ebx; \
      popl %ecx; \
      popl %edx; \
      popl %esi; \
      popl %edi; \
      popl %ebp; \
      popl %eax; \
      popl %ds; \
      popl %es; \
      popl %fs; \
      popl %gs
      
#define EOI \
    movb $0x20,%al ; \
    outb %al,$0x20 ;

ENTRY(keyboard_handler)
        SAVE_ALL
        call keyboard_routine
        EOI
        RESTORE_ALL
        iret

ENTRY(system_call_handler)
    SAVE_ALL
    cmpl  $0, %eax // Is syscall number negative?
    jl err // If it is, jump to return an error
    cmpl $MAX_SYSCALL, %eax // Is syscall greater than MAX_SYSCALL (4)?
    jg err // If it is, jump to return an error
    call *sys_call_table(,%eax,0x04) //Call the corresponding service routine
    jmp fin // Finish
err:
    movl $-32, %eax // Move to eax the ENOSYS error
fin:
    movl %eax,0x18(%esp) // Change the eax value in the stack
    RESTORE_ALL
    iret
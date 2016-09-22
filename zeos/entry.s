# 1 "entry.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "entry.S"




# 1 "include/asm.h" 1
# 6 "entry.S" 2
# 1 "include/segment.h" 1
# 7 "entry.S" 2
# 72 "entry.S"
keyboard_handler:
        pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
        movb $0x20,%al ; outb %al,$0x20 ;
        call keyboard_routine
        movl %gs,28(%esp); movl %fs,24(%esp); movl %es,20(%esp); movl %ds,1C(%esp); movl %eax,18(%esp); movl %ebp,14(%esp); movl %edi,10(%esp); movl %esi,C(%esp); movl %edx,8(%esp); movl %ecx,4(%esp); movl %ebx,(%esp)
        ret

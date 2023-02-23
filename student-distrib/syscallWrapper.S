#define ASM 1

.globl syscall_wrapper

jumptable:
    .long 0x0
    .long syscall_halt
    .long syscall_execute
    .long syscall_read
    .long syscall_write
    .long syscall_open
    .long syscall_close
    .long syscall_getargs
    .long syscall_vidmap
    .long syscall_set_handler
    .long syscall_sigreturn

syscall_wrapper:
    pushl %ebp
    pushl %esi
    pushl %edi

    pushl %edx
    pushl %ecx
    pushl %ebx

    cmp $1, %eax
    jl fail
    cmp $10, %eax
    jg fail

    call *jumptable(, %eax, 4)
    popfl
    iret

fail:
    movl $-1, %eax
    iret

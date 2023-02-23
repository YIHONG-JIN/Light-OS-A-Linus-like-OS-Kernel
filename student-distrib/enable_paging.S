#define ASM 1
.globl paging_enable


paging_enable:

    pushl %ebp
    movl %esp, %ebp

    pushl %esi
    pushl %ebx
    pushl %edi
    #loading each value in specific register
    # load the array of pde which contains the pointer to the directory
    # address is 8 below
    xorl %ecx, %ecx
    movl 8(%ebp), %ecx
    movl %ecx, %cr3

    xorl %edx, %edx
    movl %cr4, %edx
    orl $0x00000090, %edx
    movl %edx, %cr4

    xorl %eax, %eax
    movl %cr0, %eax
    orl $0x80000001, %eax
    movl %eax, %cr0

    popl %edi
    popl %ebx
    popl %esi

    leave 
    ret



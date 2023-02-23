//
// Created by JIN01 on 3/18/2022.
//

#include "exceptions.h"

char *exceptions[20] = {
        "Divide-by-zero Error",
        "Debug",
        "Non-maskable Interrupt",
        "Breakpoint",
        "Overflow",
        "Bound Range Exceeded",
        "Invalid Opcode",
        "Device Not Available",
        "Double Fault",
        "Coprocessor Segment Overrun",
        "Invalid TSS",
        "Segment Not Present",
        "Stack-Segment Fault",
        "General Protection Fault",
        "Page Fault",
        "Reserved",
        "x87 Floating-Point Exception",
        "Alignment Check",
        "Machine Check",
        "SIMD Floating-Point Exception"
};

// void exception_handler(uint32_t vectorNumber)
// input: uint32_t vectorNumber, the index of string which should be printed out
// output: none
// functionality: print out the exception message
void exception_handler(uint32_t vectorNumber) {
    cli();                                             // clear interrupt
    blue_screen();
    printf("%s\n", exceptions[vectorNumber]);     // set interrupt
    asm volatile (".1: hlt; jmp .1;");                 // loop
}

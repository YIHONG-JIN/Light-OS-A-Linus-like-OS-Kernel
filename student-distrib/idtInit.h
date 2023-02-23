//
// Created by JIN01 on 3/17/2022.
//

#ifndef MP3_IDTINIT_H
#define MP3_IDTINIT_H

#include "x86_desc.h"
#include "exceptionWrapper.h"
#include "interruptWrapper.h"
#include "syscallWrapper.h"

#define DIVIDE_ERROR                    0x00
#define DEBUG                           0x01
#define NMI_INTERRUPT                   0x02
#define BREAKPOINT                      0x03
#define OVERFLOW                        0x04
#define BOUND_RANGE_EXCEEDED            0x05
#define INVALID_OPCODE                  0x06
#define DEVICE_NOT_AVAILABLE            0x07
#define DOUBLE_FAULT                    0x08
#define COPROCESSOR_SEGMENT_OVERRUN     0x09
#define INVALID_TSS                     0x0A
#define SEGMENT_NOT_PRESENT             0x0B
#define STACK_SEGMENT_FAULT             0x0C
#define GENERAL_PROTECTION              0x0D
#define PAGE_FAULT                      0x0E
// 0x0F, reserved
#define FPU_FLOATING_POINT_ERROR        0x10
#define ALIGNMENT_CHECK                 0x11
#define MACHINE_CHECK                   0x12
#define SIMD_FLOATING_POINT_EXCEPTION   0x13


#define IRQ_START                       0x20
#define IRQ_END                         0x2F

// devices
#define IRQ0_PIT                        0x20
#define IRQ1_KEYBOARD                   0x21
#define IRQ8_REAL_TIME_CLOCK            0x28

#define IRQ_PIT                         0
#define IRQ_NUMBER_KEYBOARD             1
#define IRQ_NUMBER_REAL_TIME_CLOCK      8


#define SYSTEM_CALL                     0x80
#define LARGEST_VECTOR                  0xFF

#ifndef ASM

// idt_init init idt table
void idt_init();

#endif

#endif //MP3_IDTINIT_H

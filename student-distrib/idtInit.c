//
// Created by JIN01 on 3/17/2022.
//

#include "idtInit.h"


// idt_init() is used to set up the IDT.
// input: none
// output: none
void idt_init() {
    int index;
    for (index = 0; index <= LARGEST_VECTOR; index++) {
        // disable all, and then enable while SET_IDT_ENTRY
        idt[index].present = 0;

        // 32 bits
        idt[index].size = 1;

        // kernel code space
        idt[index].seg_selector = KERNEL_CS;

        // priority level, system call is 3, others are 0
        if (index == SYSTEM_CALL) {
            idt[index].dpl = 3;
        } else {
            idt[index].dpl = 0;
        }

        // reserved 0~4
        idt[index].reserved0 = 0;
        idt[index].reserved1 = 1;
        idt[index].reserved2 = 1;
        // judge if IRQ
//        if (index >= IRQ_START && index <= IRQ_END) {
//            idt[index].reserved3 = 0;
//        } else {
//            idt[index].reserved3 = 1;
//        }
        idt[index].reserved3 = 1;
        idt[index].reserved4 = 0;
    }

    // exceptions
    SET_IDT_ENTRY(idt[DIVIDE_ERROR], exception_divide_by_zero);
    SET_IDT_ENTRY(idt[DEBUG], exception_debug);
    SET_IDT_ENTRY(idt[NMI_INTERRUPT], exception_non_maskable_interrupt);
    SET_IDT_ENTRY(idt[BREAKPOINT], exception_breakpoint);
    SET_IDT_ENTRY(idt[OVERFLOW], exception_overflow);
    SET_IDT_ENTRY(idt[BOUND_RANGE_EXCEEDED], exception_bound_range_exceeded);
    SET_IDT_ENTRY(idt[INVALID_OPCODE], exception_invalid_opcode);
    SET_IDT_ENTRY(idt[DEVICE_NOT_AVAILABLE], exception_device_not_available);
    SET_IDT_ENTRY(idt[DOUBLE_FAULT], exception_double_fault);
    SET_IDT_ENTRY(idt[COPROCESSOR_SEGMENT_OVERRUN], exception_coprocessor_segment_overrun);
    SET_IDT_ENTRY(idt[INVALID_TSS], exception_invalid_tss);
    SET_IDT_ENTRY(idt[SEGMENT_NOT_PRESENT], exception_segment_not_present);
    SET_IDT_ENTRY(idt[STACK_SEGMENT_FAULT], exception_stack_segment_fault);
    SET_IDT_ENTRY(idt[GENERAL_PROTECTION], exception_general_protection);
    SET_IDT_ENTRY(idt[PAGE_FAULT], exception_page_fault);
    SET_IDT_ENTRY(idt[FPU_FLOATING_POINT_ERROR], exception_fpu_floating_point_error);
    SET_IDT_ENTRY(idt[ALIGNMENT_CHECK], exception_alignment_check);
    SET_IDT_ENTRY(idt[MACHINE_CHECK], exception_machine_check);
    SET_IDT_ENTRY(idt[SIMD_FLOATING_POINT_EXCEPTION], exception_simd_floating_point_exception);

    // interrupts
    SET_IDT_ENTRY(idt[IRQ0_PIT], pit_handler_linkage);
    SET_IDT_ENTRY(idt[IRQ1_KEYBOARD], keyboard_handler_linkage);
    SET_IDT_ENTRY(idt[IRQ8_REAL_TIME_CLOCK], rtc_handler_linkage);


    // system calls
    SET_IDT_ENTRY(idt[SYSTEM_CALL], syscall_wrapper);

    lidt(idt_desc_ptr);
};


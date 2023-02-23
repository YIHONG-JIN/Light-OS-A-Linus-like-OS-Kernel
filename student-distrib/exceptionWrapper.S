#define ASM 1
#include "exceptionWrapper.h"

#define EXCEPTION_WRAPPER(name, vectorNumber)    \
.GLOBL name                                     ;\
name:                                           ;\
    pushl $vectorNumber                         ;\
    call exception_handler                      ;\
    add $4, %esp                                ;\
    iret

// These functions are assembly linkage of exception_handler

EXCEPTION_WRAPPER(exception_divide_by_zero, DIVIDE_ERROR);
EXCEPTION_WRAPPER(exception_debug, DEBUG);
EXCEPTION_WRAPPER(exception_non_maskable_interrupt, NMI_INTERRUPT);
EXCEPTION_WRAPPER(exception_breakpoint, BREAKPOINT);
EXCEPTION_WRAPPER(exception_overflow, OVERFLOW);
EXCEPTION_WRAPPER(exception_bound_range_exceeded, BOUND_RANGE_EXCEEDED);
EXCEPTION_WRAPPER(exception_invalid_opcode, INVALID_OPCODE);
EXCEPTION_WRAPPER(exception_device_not_available, DEVICE_NOT_AVAILABLE);
EXCEPTION_WRAPPER(exception_double_fault, DOUBLE_FAULT);
EXCEPTION_WRAPPER(exception_coprocessor_segment_overrun, COPROCESSOR_SEGMENT_OVERRUN);
EXCEPTION_WRAPPER(exception_invalid_tss, INVALID_TSS);
EXCEPTION_WRAPPER(exception_segment_not_present, SEGMENT_NOT_PRESENT);
EXCEPTION_WRAPPER(exception_stack_segment_fault, STACK_SEGMENT_FAULT);
EXCEPTION_WRAPPER(exception_general_protection, GENERAL_PROTECTION);
EXCEPTION_WRAPPER(exception_page_fault, PAGE_FAULT);
EXCEPTION_WRAPPER(exception_fpu_floating_point_error, FPU_FLOATING_POINT_ERROR);
EXCEPTION_WRAPPER(exception_alignment_check, ALIGNMENT_CHECK);
EXCEPTION_WRAPPER(exception_machine_check, MACHINE_CHECK);
EXCEPTION_WRAPPER(exception_simd_floating_point_exception, SIMD_FLOATING_POINT_EXCEPTION);



//
// Created by JIN01 on 3/18/2022.
//

#ifndef MP3_EXCEPTIONWRAPPER_H
#define MP3_EXCEPTIONWRAPPER_H

#include "idtInit.h"

#ifndef ASM

void exception_divide_by_zero();

void exception_debug();

void exception_non_maskable_interrupt();

void exception_breakpoint();

void exception_overflow();

void exception_bound_range_exceeded();

void exception_invalid_opcode();

void exception_device_not_available();

void exception_double_fault();

void exception_coprocessor_segment_overrun();

void exception_invalid_tss();

void exception_segment_not_present();

void exception_stack_segment_fault();

void exception_general_protection();

void exception_page_fault();

void exception_fpu_floating_point_error();

void exception_alignment_check();

void exception_machine_check();

void exception_simd_floating_point_exception();

#endif

#endif //MP3_EXCEPTIONWRAPPER_H

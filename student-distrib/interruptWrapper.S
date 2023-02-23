#define ASM 1

#include "interruptWrapper.h"

#define INTERRUPT_WRAPPER(name, func)    \
.GLOBAL name                            ;\
name:                                   ;\
    pushal                              ;\
    pushfl                              ;\
    call func                           ;\
    popfl                               ;\
    popal                               ;\
    iret

// These functions are linkage for rtc and keyboard handler
INTERRUPT_WRAPPER(pit_handler_linkage, pit_handler);
INTERRUPT_WRAPPER(rtc_handler_linkage, rtc_handler);
INTERRUPT_WRAPPER(keyboard_handler_linkage, keyboard_handler);

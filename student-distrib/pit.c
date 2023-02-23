//
// Created by JIN01 on 4/20/2022.
//

// The Programmable Interval Timer (PIT) chip (Intel 8253/8254) basically consists of an oscillator, a prescaler and 3 independent frequency dividers. Each frequency divider has an output, which is used to allow the timer to control external circuitry (for example, IRQ 0).
// This driver is written by referencing from https://wiki.osdev.org/Programmable_Interval_Timer

#include "pit.h"

// pit_init()
// Input: none
// Output: none
// functionality: initialize the pit
void pit_init(void) {
    outb(PIT_MODE_3, PIT_COMMAND);
    outb(PIT_FREQ & 0xFF, PIT_CHANNEL_0);
    outb(PIT_FREQ >> 8, PIT_CHANNEL_0);

    enable_irq(IRQ_PIT);
    return;
}

// pit_handler()
// Input: none
// Output: none
// functionality: handle the pit interrupt, call scheduler
void pit_handler(void) {
    cli();
    send_eoi(IRQ_PIT);
    schedule();
    sti();
    return;
}

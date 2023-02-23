//
// Created by JIN01 on 4/20/2022.
//

#ifndef MP3_PIT_H
#define MP3_PIT_H

#include "lib.h"
#include "i8259.h"
#include "idtInit.h"
#include "syscall.h"

#define PIT_FREQ        1193182 / 100

#define PIT_CHANNEL_0   0x40
#define PIT_COMMAND     0x43
#define PIT_MODE_3      0x36

void pit_init(void);

void pit_handler(void);

extern terminal_t *active_terminal;
extern uint32_t active_pid;

#endif //MP3_PIT_H

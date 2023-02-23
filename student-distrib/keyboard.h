//
// Created by JIN01 on 3/19/2022.
//

#ifndef MP3_KEYBOARD_H
#define MP3_KEYBOARD_H

#include "lib.h"
#include "i8259.h"
#include "idtInit.h"
#include "syscall.h"

#define KEYBOARD_PORT   0x60

#define KEYBOARD_MODE_NUMBER    2
#define KEYBOARD_KEY_NUMBER     58
#define KEYBOARD_BUFFER_SIZE    128

#define LALT_PRESSED            0x38
#define LALT_RELEASED           0xB8

#define LCTRL_PRESSED           0x1D
#define LCTRL_RELEASED          0x9D

#define LSHIFT_PRESSED          0x2A
#define LSHIFT_RELEASED         0xAA
#define RSHIFT_PRESSED          0x36
#define RSHIFT_RELEASED         0xB6

#define CAPS                    0x3A
#define BACKSPACE               0x0E


#define F1                      0x3B
#define F2                      0x3C
#define F3                      0x3D
//#define VIDEO_ADDRESS_BASE      0xB8000
//#define VIDEO_ADDRESS_TO_PHYSICAL     0xB7000

//extern volatile int keyboard_buffer_size;
//extern volatile int keyboard_buffer_enable;
//extern uint8_t key_buf[128];

void keyboard_init();

void keyboard_handler();

//typedef struct terminal {
//    int terminal_id;
//    int screen_x;
//    int screen_y;
//    uint8_t key_buf[128];
//    volatile int keyboard_buffer_size;
//    volatile int keyboard_buffer_enable;
//    volatile int overflow;
//    char* video_address;
//    uint32_t current_process_id;
//
//    volatile int32_t virtual_rate;
//    volatile int32_t virtual_count;
//} terminal_t;

//extern terminal_t terminal_1;
//extern terminal_t terminal_2;
//extern terminal_t terminal_3;
//
//extern terminal_t* current_terminal;

void switch_to_terminal_1();

void switch_to_terminal_2();

void switch_to_terminal_3();

#endif //MP3_KEYBOARD_H

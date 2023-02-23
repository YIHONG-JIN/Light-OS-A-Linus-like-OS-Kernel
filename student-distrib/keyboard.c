//
// Created by JIN01 on 3/19/2022.
//

// The PS/2 Keyboard is a device that talks to a PS/2 controller using serial communication. Ideally, each different type of PS/2 controller driver should provide some sort of standard/simple "send byte/receive byte" interface, and the PS/2 Keyboard driver would use this interface without caring about lower level details (like what type of PS/2 controller the device is plugged into).
// The PS/2 Keyboard accepts commands and sends responses to those commands, and also sends scan codes indicating when a key was pressed or released. Keep in mind that these scan codes (from the tables below) may be altered (i.e. translated) by the PS/2 controller, depending on the controller's settings.
// This driver is written by referencing from https://wiki.osdev.org/PS/2_Keyboard


#include "keyboard.h"

static uint8_t scancode[KEYBOARD_MODE_NUMBER][KEYBOARD_KEY_NUMBER] = {
        {'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
                'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', '\0', 'a', 's',
                'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
                'b', 'n', 'm', ',', '.', '/', '\0', '\0', '\0', ' '},
        {'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
                'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', '\0', 'A', 'S',
                'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"',  '~', '\0', '|',  'Z', 'X', 'C', 'V',
                'B', 'N', 'M', '<', '>', '?', '\0', '\0', '\0', ' '}
};

//uint8_t key_buf[128];
//volatile int keyboard_buffer_size = 0;
//volatile int keyboard_buffer_enable = 0;
//uint32_t overflow = 0;
uint32_t alt_pressed = 0;
uint32_t ctrl_pressed = 0;
uint32_t shift_pressed = 0;
uint32_t caps_lock = 0;

// KEYBOARD INTERRUPT INITIALIZATION
// Input: none
// Output: none
// Description: enable keyboard ability to send interrupts in order to write characters on screen
void keyboard_init() {
    terminal_1.terminal_id = 1;
    terminal_1.screen_x = 0;
    terminal_1.screen_y = 0;
    terminal_1.keyboard_buffer_size = 0;
    terminal_1.keyboard_buffer_enable = 0;
    terminal_1.overflow = 0;
    terminal_1.video_address = (char *) VIDEO_MEM + 4096 * 1;
    terminal_1.current_process_id = 0;
    terminal_1.virtual_rate = 2;

    terminal_2.terminal_id = 2;
    terminal_2.screen_x = 0;
    terminal_2.screen_y = 0;
    terminal_2.keyboard_buffer_size = 0;
    terminal_2.keyboard_buffer_enable = 0;
    terminal_2.overflow = 0;
    terminal_2.video_address = (char *) VIDEO_MEM + 4096 * 2;
    terminal_2.current_process_id = 0;
    terminal_2.virtual_rate = 2;

    terminal_3.terminal_id = 3;
    terminal_3.screen_x = 0;
    terminal_3.screen_y = 0;
    terminal_3.keyboard_buffer_size = 0;
    terminal_3.keyboard_buffer_enable = 0;
    terminal_3.overflow = 0;
    terminal_3.video_address = (char *) VIDEO_MEM + 4096 * 3;
    terminal_3.current_process_id = 0;
    terminal_3.virtual_rate = 2;

    enable_irq(IRQ_NUMBER_KEYBOARD);                              // enable keyboard interrupt
}

// KEYBOARD INTERRUPT HANDLER
// Input: none
// Output: none
// Description: service the keyboard interrupt and print the correct character, then end interrupt
void keyboard_handler() {
    uint8_t input = inb(KEYBOARD_PORT);
    char key;
    if (input == LALT_PRESSED) {
        alt_pressed = 1;
    } else if (input == LALT_RELEASED) {
        alt_pressed = 0;
    } else if (input == LCTRL_PRESSED) {
        ctrl_pressed = 1;
    } else if (input == LCTRL_RELEASED) {
        ctrl_pressed = 0;
    } else if (input == LSHIFT_PRESSED) {
        shift_pressed = 1;
    } else if (input == LSHIFT_RELEASED) {
        shift_pressed = 0;
    } else if (input == RSHIFT_PRESSED) {
        shift_pressed = 1;
    } else if (input == RSHIFT_RELEASED) {
        shift_pressed = 0;
    } else if (input == CAPS) {
        caps_lock |= 1;
    } else if (input == BACKSPACE) {
        if (current_terminal->overflow > 0) {
            current_terminal->overflow--;
        } else if (current_terminal->keyboard_buffer_size > 0) {
            current_terminal->keyboard_buffer_size--;
            current_terminal_back_putc();
        }
    } else if (input < KEYBOARD_KEY_NUMBER) {
        key = scancode[shift_pressed | caps_lock][input];
        current_terminal_putc(key);
        if ((key == 'l' || key == 'L') && ctrl_pressed) {
            current_terminal_clear();
            current_terminal->keyboard_buffer_size = 0;
        } else if (current_terminal->keyboard_buffer_size >= KEYBOARD_BUFFER_SIZE &&
                   current_terminal->keyboard_buffer_enable && key == '\n') {
            current_terminal->key_buf[KEYBOARD_BUFFER_SIZE - 1] = '\n';
            current_terminal->keyboard_buffer_enable = 0;
            current_terminal->overflow++;
        } else if (key == '\n' && current_terminal->keyboard_buffer_enable) {
            current_terminal->key_buf[current_terminal->keyboard_buffer_size] = '\n';
            current_terminal->keyboard_buffer_size++;
            current_terminal->keyboard_buffer_enable = 0;
        } else if (current_terminal->keyboard_buffer_enable) {
            current_terminal->key_buf[current_terminal->keyboard_buffer_size] = key;
            current_terminal->keyboard_buffer_size++;
        }
    } else if (input == F1 && alt_pressed) {
        switch_to_terminal_1();
    } else if (input == F2 && alt_pressed) {
        switch_to_terminal_2();
    } else if (input == F3 && alt_pressed) {
        switch_to_terminal_3();
    }
    send_eoi(IRQ_NUMBER_KEYBOARD);
}

// switch_to_terminal_1
// Input: none
// Output: none
// Description: switch to terminal 1
void switch_to_terminal_1() {
    if (current_terminal == &terminal_1) {
        return;
    }
    cli();

    memcpy(current_terminal->video_address, (char *) VIDEO_ADDRESS_TO_PHYSICAL, 4096);

    current_terminal = &terminal_1;

    paging_video_memory();

    memcpy((char *) VIDEO_ADDRESS_TO_PHYSICAL, current_terminal->video_address, 4096);

    set_cursor_position(current_terminal->screen_x, current_terminal->screen_y);

    sti();
}

// switch_to_terminal_2
// Input: none
// Output: none
// Description: switch to terminal 2
void switch_to_terminal_2() {
    if (current_terminal == &terminal_2) {
        return;
    }
    cli();

    memcpy(current_terminal->video_address, (char *) VIDEO_ADDRESS_TO_PHYSICAL, 4096);

    current_terminal = &terminal_2;

    memcpy((char *) VIDEO_ADDRESS_TO_PHYSICAL, current_terminal->video_address, 4096);

    paging_video_memory();

    set_cursor_position(current_terminal->screen_x, current_terminal->screen_y);

    sti();
}

// switch_to_terminal_3
// Input: none
// Output: none
// Description: switch to terminal 3
void switch_to_terminal_3() {
    if (current_terminal == &terminal_3) {
        return;
    }
    cli();

    memcpy(current_terminal->video_address, (char *) VIDEO_ADDRESS_TO_PHYSICAL, 4096);

    current_terminal = &terminal_3;

    paging_video_memory();

    memcpy((char *) VIDEO_ADDRESS_TO_PHYSICAL, current_terminal->video_address, 4096);

    set_cursor_position(current_terminal->screen_x, current_terminal->screen_y);

    sti();
}

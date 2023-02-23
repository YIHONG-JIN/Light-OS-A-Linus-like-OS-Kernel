//
// Created by JIN01 on 3/27/2022.
//

#ifndef MP3_TERMINAL_H
#define MP3_TERMINAL_H

#include "keyboard.h"

void keyboard_handler_main(void);

int32_t terminal_open(int32_t *fd, char *filename);

int32_t terminal_close(int32_t *fd);

int32_t terminal_read(int32_t fd, int8_t *buf, uint32_t n, uint32_t *offset);

int32_t terminal_write(int32_t fd, int8_t *buf, uint32_t n, uint32_t *offset);

extern file_ops_t terminal_file_ops_stdin;
extern file_ops_t terminal_file_ops_stdout;

#endif //MP3_TERMINAL_H

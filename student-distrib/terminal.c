#include "terminal.h"
// --------------------------------------------
// Next functions to be put in terminal.c
// --------------------------------------------

// terminal_file_ops_stdout is a file_ops_t struct that is used to read from terminal
file_ops_t terminal_file_ops_stdin = {
        .open = terminal_open,
        .read = terminal_read,
        .write = NULL,
        .close = terminal_close
};

// terminal_file_ops_stdin is a file_ops_t struct that is used to write into terminal
file_ops_t terminal_file_ops_stdout = {
        .open = terminal_open,
        .read = NULL,
        .write = terminal_write,
        .close = terminal_close
};

// OPEN TERMINAL
// Input: none
// Output: none
// Description: sets size of the buffer and its enable bit to 0
int32_t terminal_open(int32_t *fd, char *filename) {
    active_terminal->keyboard_buffer_size = 0;
    active_terminal->keyboard_buffer_enable = 0;
    return 0;
}

// CLOSE TERMINAL
// Input: none
// Output: none
// Description: sets size of the buffer and its enable bit to 0
int32_t terminal_close(int32_t *fd) {
    active_terminal->keyboard_buffer_size = 0;
    active_terminal->keyboard_buffer_enable = 0;
    return 0;
}

// READ TERMINAL
// Input: file descriptor, buffer, size of buffer
// Output: number of bits read
// Description: copies keyboard buffer to buffer
int32_t terminal_read(int32_t fd, int8_t *buf, uint32_t n, uint32_t *offset) {
    if (buf == NULL) return -1;                                    // if there is no keyboard buffer return -1 (failure)
    if (n == 0) return 0;                                          // if keyboard buffer is empty return 0
    active_terminal->keyboard_buffer_enable = 1;
    while (active_terminal->keyboard_buffer_enable == 1) {};
    int index;
    if (n > active_terminal->keyboard_buffer_size)
        n = active_terminal->keyboard_buffer_size;        // if keyboard buffer overflows make its size 128
    for (index = 0; index <
                    n; index++) {                          // copy keyboard buffer to buffer and make sure there is '\n' in the end
        buf[index] = active_terminal->key_buf[index];
        if (buf[index] == '\n') {
            active_terminal->keyboard_buffer_size = 0;
            return index + 1;
        }
    }
    buf[n - 1] = '\n';
    active_terminal->keyboard_buffer_size = 0;
    return n;
};

// WRITE TERMINAL
// Input: file descriptor, buffer, size of buffer
// Output: number of bits written
// Description: writes buffer to console
int32_t terminal_write(int32_t fd, int8_t *buf, uint32_t n, uint32_t *offset) {
    // if there is no buffer return -1 (failure)
    if (buf == NULL) {
        return -1;
    }
    // if buffer is empty return 0
    if (n == 0) {
        return 0;
    }

    int index;
    // write each character to console until new line
    for (index = 0; index < n; index++) {
        putc(buf[index]);
    }
    return n;
};


//Created by johndd2 on 4/6/2022
#ifndef MP3_SYSCALL_H
#define MP3_SYSCALL_H

#include "idtInit.h"
#include "lib.h"
#include "filesystem.h"
#include "x86_desc.h"
#include "paging.h"
#include "terminal.h"
#include "keyboard.h"

#define USER_STACK_BASE_ADDRESS         0x08400000 - 0x4
#define VIRTUAL_ADDRESS_PROGRAM_START   0x08048000
#define VIRTUAL_128MB_START             0x08000000
#define KERNEL_STACK_BASE_ADDRESS       0x800000
#define USER_KERNEL_STACK_SIZE          0x2000

#define MAX_ARG_LENGTH                  33
#define MAX_NUM_PROCESSES               6


typedef struct terminal {
    int terminal_id;
    int screen_x;
    int screen_y;
    uint8_t key_buf[128];
    volatile int keyboard_buffer_size;
    volatile int keyboard_buffer_enable;
    volatile int overflow;
    char *video_address;
    uint32_t current_process_id;

    volatile int32_t virtual_rate;
    volatile int32_t virtual_count;
} terminal_t;

typedef struct pcb {
    file_descriptor_t file_descriptor_array[MAX_NUM_FILES];
    uint32_t current_pid;
    uint32_t parent_pid;

    uint32_t current_esp;
    uint32_t current_ebp;
    uint32_t current_eip;

    uint8_t arg_buf[MAX_ARG_LENGTH];
    // corresponding terminal
    uint32_t vidmap_flag;
    terminal_t *terminal;
} pcb_t;

int32_t syscall_halt(uint8_t status);

int32_t syscall_execute(const uint8_t *command);

int32_t syscall_read(int32_t fd, void *buf, int32_t nbytes);

int32_t syscall_write(int32_t fd, const void *buf, int32_t nbytes);

int32_t syscall_open(const uint8_t *filename);

int32_t syscall_close(int32_t fd);

int32_t syscall_getargs(uint8_t *buf, int32_t nbytes);

int32_t syscall_vidmap(uint8_t **screen_start);

int32_t syscall_set_handler(int32_t signum, void *handler_address);

int32_t syscall_sigreturn(void);

void paging_execute(uint32_t pid);

void paging_halt(uint32_t pid);

void paging_video_memory();

void context_switch_execute();

void context_switch_halt(uint8_t status);

void syscall_enable_vidmap_paging();

void syscall_disable_vidmap_paging();

void schedule();

void create_terminal_1_shell();

void create_terminal_2_shell();

void create_terminal_3_shell();

void boot_enter_terminal_1();

void boot_enter_terminal_2();

void boot_enter_terminal_3();

extern terminal_t terminal_1;
extern terminal_t terminal_2;
extern terminal_t terminal_3;

extern terminal_t *current_terminal;

extern terminal_t *active_terminal;

extern uint32_t active_pid;
#endif //MP#_SYSCALL_H

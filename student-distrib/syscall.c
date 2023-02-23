//Created by johndd2 on 4/6/2022

#include "syscall.h"

terminal_t *active_terminal = &terminal_1;
uint32_t active_pid = 0;
int32_t process_count = 3;

// process pool
// process 0 is kernel
// process 1 is shell for terminal 1
// process 2 is shell for terminal 2
// process 3 is shell for terminal 3
int32_t processes[7] = {1, 1, 1, 1, 0, 0, 0};

extern terminal_t *current_terminal;

uint8_t arg_buf[MAX_ARG_LENGTH];
uint8_t argument_exist = 0;


terminal_t terminal_1;
terminal_t terminal_2;
terminal_t terminal_3;

terminal_t *current_terminal = &terminal_1;

// get_process_available: returns the next available process
// returns -1 if no process is available
int32_t get_process_available() {
    int32_t i;
    for (i = 0; i < 7; i++) {
        if (processes[i] == 0) {
            processes[i] = 1;
            return i;
        }
    }
    return -1;
}

// release_process: releases the process
// returns -1 if process is not valid
// input: process_id
// output: status
int32_t release_process(int32_t pid) {
    if (pid < 0 || pid > 6) {
        return -1;
    }
    processes[pid] = 0;
    return 0;
}

// HALT
// Input: status
// Output: success/fail
// Description: halts process 
int32_t syscall_halt(uint8_t status) {
    pcb_t *pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - active_pid * USER_KERNEL_STACK_SIZE);

    if (active_pid == 1) {
        printf(" root shell of terminal 1 could be be halted\n");
        boot_enter_terminal_1();
        return 0;
    }

    if (active_pid == 2) {
        printf(" root shell of terminal 2 could be be halted\n");
        boot_enter_terminal_2();
        return 0;
    }

    if (active_pid == 3) {
        printf(" root shell of terminal 3 could be be halted\n");
        boot_enter_terminal_3();
        return 0;
    }

    process_count--;
    release_process(active_pid);

    int i;
    for (i = 0; i < MAX_NUM_FILES; i++) {
        file_descriptor_table_close(pcb->file_descriptor_array, i);
    }

    int parent_pid = pcb->parent_pid;
    active_pid = parent_pid;
    pcb_t *parent_pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - active_pid * USER_KERNEL_STACK_SIZE);
    active_terminal = parent_pcb->terminal;
    active_terminal->current_process_id = active_pid;

    uint32_t stack_pointer = KERNEL_STACK_BASE_ADDRESS - (active_pid - 1) * USER_KERNEL_STACK_SIZE - 0x04;
    tss.esp0 = stack_pointer;

    paging_execute(active_pid);
    paging_video_memory();

    context_switch_halt(status);

    return 0;
}

// EXECUTE
// Input: command
// Output: success/fail
// Description: executes/runs file
int32_t syscall_execute(const uint8_t *command) {
    if (process_count == MAX_NUM_PROCESSES) {
        printf("Max number of processes reached\n");
        return -1;
    }

    argument_exist = 0;
    int32_t fd = -1;     // inode
    int8_t buf[4];
    int len = strlen((int8_t *) command) + 1;
    if (len > MAX_ARG_LENGTH + 4) {
        printf("Command is too long\n");
        return -1;
    }
    uint8_t fn[33];      // max file name length - 32
    memset(arg_buf, 0, MAX_ARG_LENGTH);

    int flag = 0;
    int fn_idx = 0;
    int i;
    for (i = 0; i < len; i++) {
        if (command[i] == '\0') {
            fn[fn_idx++] = '\0';
            break;
        }
        if (command[i] == ' ' && flag) {
            fn[fn_idx++] = '\0';
            flag = 0;
            break;
        } else if (command[i] != ' ') {
            flag = 1;
            fn[fn_idx++] = command[i];
        }
    }

    int arg_idx = 0;
    flag = 0;
    for (; i < len; i++) {
        if (command[i] == '\0') {
            arg_buf[arg_idx++] = '\0';
            break;
        }
        if (command[i] == ' ' && flag) {
            arg_buf[arg_idx++] = '\0';
            flag = 0;
            break;
        } else if (command[i] != ' ') {
            flag = 1;
            arg_buf[arg_idx++] = command[i];
            argument_exist = 1;
        }
    }

    // check if it is a file
    dir_entry_t current_directory_entry;
    if (read_dentry_by_name((int8_t *) fn, &current_directory_entry) == -1) {
        return -1;
    }
    if (current_directory_entry.filetype != TYPE_FILE) {
        return -1;
    }

    // check if the file is executable
    if (file_open(&fd, (char *) fn) == -1) return -1;
    if (read_data(fd, 0, buf, 4) == -1)
        return -1;           // check first 4 bytes to make sure file is executable (=> offset = 0, length = 4)
    if ((buf[0] != 0x7f) || (buf[1] != 0x45) || (buf[2] != 0x4c) ||
        (buf[3] != 0x46)) {     // special values which executable files have
        return -1;
    }

    process_count++;
    int32_t new_process_id = get_process_available();
    if (new_process_id == -1) {
        printf("Max number of processes reached\n");
        return -1;
    }
    pcb_t *new_process_pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - new_process_id * USER_KERNEL_STACK_SIZE);
    file_descriptor_table_init(new_process_pcb->file_descriptor_array);
    new_process_pcb->current_pid = new_process_id;
    new_process_pcb->parent_pid = active_pid;
    new_process_pcb->current_esp = USER_STACK_BASE_ADDRESS;
    new_process_pcb->current_ebp = USER_STACK_BASE_ADDRESS;
    new_process_pcb->current_eip = (*(uint32_t *) (VIRTUAL_ADDRESS_PROGRAM_START + 24));
    new_process_pcb->vidmap_flag = 0;
    new_process_pcb->terminal = active_terminal;
    memcpy(new_process_pcb->arg_buf, arg_buf, MAX_ARG_LENGTH);

    active_terminal->current_process_id = new_process_id;

    paging_execute(new_process_id);
    paging_video_memory();

    read_data(fd, 0, (char *) VIRTUAL_ADDRESS_PROGRAM_START,
              MAX_FILE_SIZE);   // read the whole file data (=> offset = 0, length = MAX_FILE_SIZE)

    pcb_t *current_pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - (active_pid * USER_KERNEL_STACK_SIZE));
    asm volatile("movl %%esp, %0":"=r" (current_pcb->current_esp));
    asm volatile("movl %%ebp, %0":"=r" (current_pcb->current_ebp));

    active_pid = new_process_id;
    tss.esp0 = KERNEL_STACK_BASE_ADDRESS - active_pid * USER_KERNEL_STACK_SIZE;
    context_switch_execute();
    return 0;
}

// syscall_read
// Input: fd, buf, nbytes
// Output: success/fail
// description: read from file
int32_t syscall_read(int32_t fd, void *buf, int32_t nbytes) {
    pcb_t *pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - (active_pid * USER_KERNEL_STACK_SIZE));
    return file_descriptor_table_read(pcb->file_descriptor_array, fd, (int8_t *) buf, nbytes);
}

// syscall_write
// Input: fd, buf, nbytes
// Output: success/fail
// description: write to file
int32_t syscall_write(int32_t fd, const void *buf, int32_t nbytes) {
    pcb_t *pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - (active_pid * USER_KERNEL_STACK_SIZE));
    return file_descriptor_table_write(pcb->file_descriptor_array, fd, (int8_t *) buf, nbytes);
}

// syscall_open
// Input: filename
// Output: success/fail
// description: open file
int32_t syscall_open(const uint8_t *filename) {
    pcb_t *pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - (active_pid * USER_KERNEL_STACK_SIZE));
    return file_descriptor_table_open(pcb->file_descriptor_array, (char *) filename);
}

// syscall_close
// Input: fd
// Output: success/fail
// description: close file
int32_t syscall_close(int32_t fd) {
    pcb_t *pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - (active_pid * USER_KERNEL_STACK_SIZE));
    return file_descriptor_table_close(pcb->file_descriptor_array, fd);
}

// syscall_getargs
// Input: buf, nbytes
// Output: success/fail
// description: get arguments
int32_t syscall_getargs(uint8_t *buf, int32_t nbytes) {
    pcb_t *pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - (active_pid * USER_KERNEL_STACK_SIZE));
    if (argument_exist == 0) {
        return -1;
    }
    if (buf == NULL) return -1;
    int length = strlen((int8_t *) pcb->arg_buf);

    if (length > nbytes) return -1;
    int i;
    for (i = 0; i <= length; i++) {
        buf[i] = pcb->arg_buf[i];
    }

    return 0;
}

// syscall_vidmap
// Input: screen_start
// Output: success/fail
// description: map video memory for user program, set current process's vidmap_flag to 1
int32_t syscall_vidmap(uint8_t **screen_start) {
    pcb_t *pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - (active_pid * USER_KERNEL_STACK_SIZE));
    if (screen_start == NULL) {
        return -1;
    }
    if ((uint32_t) screen_start < VIRTUAL_128MB_START ||
        (uint32_t) screen_start > VIRTUAL_128MB_START + (KERNEL_STACK_BASE_ADDRESS / 2)) {
        return -1;
    }
    pcb->vidmap_flag = 1;
    syscall_enable_vidmap_paging();
    uint32_t vidmap_address = VIRTUAL_128MB_START + (KERNEL_STACK_BASE_ADDRESS / 2);
    *screen_start = (uint8_t *) vidmap_address;
    return (int32_t) vidmap_address;
}

// syscall_set_handler
// Input: signum, handler_address
// Output: success/fail
// description: set signal handler
int32_t syscall_set_handler(int32_t signum, void *handler_address) {
    return -1;
}

// syscall_sigreturn
// Input: none
// Output: success/fail
// description: return from signal handler
int32_t syscall_sigreturn(void) {
    return -1;
}


// PAGING EXECUTE
// Input: none
// Output: none
// Description: modifies USER page
void paging_execute(uint32_t pid) {
    uint32_t PD_index = (uint32_t) VIRTUAL_ADDRESS_PROGRAM_START >> (PTE_OFFSET + PAGE_OFFSET);
    page_dir[PD_index].present = 1;
    page_dir[PD_index].write_read = 1;
    page_dir[PD_index].u_s = 1;
    page_dir[PD_index].write_through = 0;
    page_dir[PD_index].cache_disable = 0;
    page_dir[PD_index].access = 0;
    page_dir[PD_index].dirty = 0;

    page_dir[PD_index].page_size = 1;
    page_dir[PD_index].glob = 0;
    page_dir[PD_index].avail = 0;
    page_dir[PD_index].address = 0x00400 + 0x00400 + pid * 0x00400;

    // change cr3 and flush TLB
    asm volatile ("              \n\
  	        movl %%cr3, %%eax    \n\
  	        movl %%eax, %%cr3    \n\
            "
    : : : "eax", "cc"
    );
    return;
}

// PAGING HALT
// Input: pid
// Output: none
// Description: halts the modified page
void paging_halt(uint32_t pid) {
    uint32_t PD_index = (uint32_t) VIRTUAL_ADDRESS_PROGRAM_START >> (PTE_OFFSET + PAGE_OFFSET);
    page_dir[PD_index].present = 1;
    page_dir[PD_index].write_read = 1;
    page_dir[PD_index].u_s = 1;
    page_dir[PD_index].write_through = 0;
    page_dir[PD_index].cache_disable = 0;
    page_dir[PD_index].access = 0;
    page_dir[PD_index].dirty = 0;

    page_dir[PD_index].page_size = 1;
    page_dir[PD_index].glob = 0;
    page_dir[PD_index].avail = 0;
    page_dir[PD_index].address = 0x00400 + 0x00400 + (pid - 1) * 0x00400;
    process_count--;
    asm volatile ("              \n\
  	        movl %%cr3, %%eax    \n\
  	        movl %%eax, %%cr3    \n\
            "
    : : : "eax", "cc"
    );
    return;
}

// paging_video_memory
// Input: none
// Output: none
// Description: modifies video memory paging
void paging_video_memory() {
    pcb_t *target_pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - active_pid * USER_KERNEL_STACK_SIZE);
    if (active_terminal == current_terminal) {
        pg_tab_1[(VIDEO_MEM) >> 12].address = VIDEO_MEM >> 12;
        vidmap_pg_tab_1[0].address = VIDEO_MEM >> 12;
    } else {
        pg_tab_1[(VIDEO_MEM) >> 12].address = (VIDEO_MEM >> 12) + active_terminal->terminal_id;
        vidmap_pg_tab_1[0].address = (VIDEO_MEM >> 12) + active_terminal->terminal_id;
    }

    if (target_pcb->vidmap_flag == 1) {
        vidmap_pg_tab_1[0].present = 1;
    } else {
        vidmap_pg_tab_1[0].present = 0;
    }

    asm volatile ("              \n\
  	        movl %%cr3, %%eax    \n\
  	        movl %%eax, %%cr3    \n\
            "
    : : : "eax", "cc"
    );
}

// syscall_enable_vidmap_paging
// Input: none
// Output: none
// Description: enables vidmap paging
void syscall_enable_vidmap_paging() {
    uint32_t address = VIRTUAL_128MB_START + (KERNEL_STACK_BASE_ADDRESS / 2);
    uint32_t PD_index = address >> (PTE_OFFSET + PAGE_OFFSET);
    page_dir[PD_index].present = 1;
    page_dir[PD_index].write_read = 1;
    page_dir[PD_index].u_s = 1;
    page_dir[PD_index].write_through = 0;
    page_dir[PD_index].cache_disable = 0;
    page_dir[PD_index].access = 0;
    page_dir[PD_index].dirty = 0;

    page_dir[PD_index].page_size = 0;
    page_dir[PD_index].glob = 0;
    page_dir[PD_index].avail = 0;
    page_dir[PD_index].address = (uint32_t) vidmap_pg_tab_1 >> 12;

    vidmap_pg_tab_1[0].present = 1;
    vidmap_pg_tab_1[0].write_read = 1;
    vidmap_pg_tab_1[0].u_s = 1;
    vidmap_pg_tab_1[0].write_through = 0;
    vidmap_pg_tab_1[0].cache_disable = 0;
    vidmap_pg_tab_1[0].access = 0;
    vidmap_pg_tab_1[0].dirty = 0;
    vidmap_pg_tab_1[0].pat = 0;
    vidmap_pg_tab_1[0].glob = 0;
    vidmap_pg_tab_1[0].avail = 0;
    vidmap_pg_tab_1[0].address = VIDEO_MEM >> 12;

    asm volatile ("              \n\
  	        movl %%cr3, %%eax    \n\
  	        movl %%eax, %%cr3    \n\
            "
    : : : "eax", "cc"
    );
    return;
}


// CONTEXT SWITCH
// Input: none
// Output: none
// Description: switches from user to kernel space
void context_switch_execute() {
    uint32_t entry_point = (*(uint32_t *) (VIRTUAL_ADDRESS_PROGRAM_START + 24));
    sti();
    asm volatile ("                \n\
            andl    $0x00FF, %%ebx \n\
            movw    %%bx, %%ds     \n\
            pushl   %%ebx          \n\
            pushl   %%edx          \n\
            pushfl                 \n\
            pushl   %%ecx          \n\
            pushl   %%eax          \n\
            iret                   \n\
            "
    :
    : "a"(entry_point), "b"(USER_DS), "c"(USER_CS), "d"(USER_STACK_BASE_ADDRESS)
    : "cc"
    );
};


// context_switch_halt
// Input: status
// Output: none
void context_switch_halt(uint8_t status) {
    pcb_t *pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - active_pid * USER_KERNEL_STACK_SIZE);
    asm volatile ("             \n\
            movl %%ecx, %%esp   \n\
            movl %%edx, %%ebp   \n\
            movl %%ebx, %%eax   \n\
            leave               \n\
            ret                 \n\
            "
    :
    : "b" (status), "c" (pcb->current_esp), "d" (pcb->current_ebp)
    : "eax", "ebp", "esp"
    );
};


// scheduler
// Input: none
// Output: none
// Description: switch to next terminal
void schedule() {
    int32_t original_pid = active_pid;
    if (active_terminal == &terminal_1) {
        active_terminal = &terminal_2;
        active_pid = active_terminal->current_process_id;
    } else if (active_terminal == &terminal_2) {
        active_terminal = &terminal_3;
        active_pid = active_terminal->current_process_id;
    } else if (active_terminal == &terminal_3 || active_terminal == NULL) {
        active_terminal = &terminal_1;
        active_pid = active_terminal->current_process_id;
    }
//    pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - active_pid * USER_KERNEL_STACK_SIZE);
    // store the stack information of the current process
    if (active_pid != 0) {
        pcb_t *pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - original_pid * USER_KERNEL_STACK_SIZE);
        asm volatile("movl %%esp, %0":"=r" (pcb->current_esp));
        asm volatile("movl %%ebp, %0":"=r" (pcb->current_ebp));
    } else {
        if (active_terminal == &terminal_1) {
            pcb_t *pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - 3 * USER_KERNEL_STACK_SIZE);
            asm volatile("movl %%esp, %0":"=r" (pcb->current_esp));
            asm volatile("movl %%ebp, %0":"=r" (pcb->current_ebp));
        } else if (active_terminal == &terminal_2) {
            pcb_t *pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - 1 * USER_KERNEL_STACK_SIZE);
            asm volatile("movl %%esp, %0":"=r" (pcb->current_esp));
            asm volatile("movl %%ebp, %0":"=r" (pcb->current_ebp));
        } else {
            pcb_t *pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - 2 * USER_KERNEL_STACK_SIZE);
            asm volatile("movl %%esp, %0":"=r" (pcb->current_esp));
            asm volatile("movl %%ebp, %0":"=r" (pcb->current_ebp));
        }
    }

    // start root shell for each terminal if it is the first time
    if (active_pid == 0) {
        if (active_terminal == &terminal_1) {
            boot_enter_terminal_1();
        } else if (active_terminal == &terminal_2) {
            boot_enter_terminal_2();
        } else if (active_terminal == &terminal_3) {
            boot_enter_terminal_3();
        }
        return;
    }

    pcb_t *target_pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - active_pid * USER_KERNEL_STACK_SIZE);

    tss.esp0 = KERNEL_STACK_BASE_ADDRESS - (active_pid - 1) * USER_KERNEL_STACK_SIZE - 0x04;
    paging_execute(active_pid);
    paging_video_memory();

    sti();
    asm volatile ("             \n\
            movl %0, %%esp      \n\
            movl %1, %%ebp      \n\
            "
    :
    : "r" (target_pcb->current_esp), "r" (target_pcb->current_ebp)
    : "memory"
    );

}

// create_terminal_1_shell
// Input: none
// Output: none
// Description: create the first terminal shell
void create_terminal_1_shell() {
    int32_t fd = -1;     // inode
    int8_t buf[4];
    char *fn = "shell";

    // check if the file is executable
    if (file_open(&fd, (char *) fn) == -1) return;
    if (read_data(fd, 0, buf, 4) == -1)
        return;           // check first 4 bytes to make sure file is executable (=> offset = 0, length = 4)
    if ((buf[0] != 0x7f) || (buf[1] != 0x45) || (buf[2] != 0x4c) ||
        (buf[3] != 0x46)) {     // special values which executable files have
        return;
    }

    // modify paging settings
    paging_execute(1);

    read_data(fd, 0, (char *) VIRTUAL_ADDRESS_PROGRAM_START,
              MAX_FILE_SIZE);   // read the whole file data (=> offset = 0, length = MAX_FILE_SIZE)

    // close the file
    file_close(&fd);

    // initialize the pcb
    pcb_t *pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - (1 * USER_KERNEL_STACK_SIZE));
    file_descriptor_table_init(pcb->file_descriptor_array);
    pcb->current_pid = 1;
    pcb->parent_pid = 0;

    pcb->current_esp = USER_STACK_BASE_ADDRESS;
    pcb->current_ebp = USER_STACK_BASE_ADDRESS;
    pcb->current_eip = (*(uint32_t *) (VIRTUAL_ADDRESS_PROGRAM_START + 24));

    pcb->vidmap_flag = 0;

    pcb->terminal = &terminal_1;
    terminal_1.current_process_id = 0;

    memset(pcb->arg_buf, 0, MAX_ARG_LENGTH);

    return;
}

// create_terminal_2_shell
// Input: none
// Output: none
// Description: create the second terminal shell
void create_terminal_2_shell() {
    int32_t fd = -1;     // inode
    int8_t buf[4];
    char *fn = "shell";

    // check if the file is executable
    if (file_open(&fd, (char *) fn) == -1) return;
    if (read_data(fd, 0, buf, 4) == -1)
        return;           // check first 4 bytes to make sure file is executable (=> offset = 0, length = 4)
    if ((buf[0] != 0x7f) || (buf[1] != 0x45) || (buf[2] != 0x4c) ||
        (buf[3] != 0x46)) {     // special values which executable files have
        return;
    }

    // modify paging settings
    paging_execute(2);

    read_data(fd, 0, (char *) VIRTUAL_ADDRESS_PROGRAM_START,
              MAX_FILE_SIZE);   // read the whole file data (=> offset = 0, length = MAX_FILE_SIZE)

    // close the file
    file_close(&fd);

    // initialize the pcb
    pcb_t *pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - (2 * USER_KERNEL_STACK_SIZE));
    file_descriptor_table_init(pcb->file_descriptor_array);
    pcb->current_pid = 2;
    pcb->parent_pid = 0;

    pcb->current_esp = USER_STACK_BASE_ADDRESS;
    pcb->current_ebp = USER_STACK_BASE_ADDRESS;
    pcb->current_eip = (*(uint32_t *) (VIRTUAL_ADDRESS_PROGRAM_START + 24));

    pcb->vidmap_flag = 0;

    pcb->terminal = &terminal_2;
    terminal_2.current_process_id = 0;

    memset(pcb->arg_buf, 0, MAX_ARG_LENGTH);

    return;
}

// create_terminal_3_shell
// Input: none
// Output: none
// Description: create the third terminal shell
void create_terminal_3_shell() {
    int32_t fd = -1;     // inode
    int8_t buf[4];
    char *fn = "shell";

    // check if the file is executable
    if (file_open(&fd, (char *) fn) == -1) return;
    if (read_data(fd, 0, buf, 4) == -1)
        return;           // check first 4 bytes to make sure file is executable (=> offset = 0, length = 4)
    if ((buf[0] != 0x7f) || (buf[1] != 0x45) || (buf[2] != 0x4c) ||
        (buf[3] != 0x46)) {     // special values which executable files have
        return;
    }

    // modify paging settings
    paging_execute(3);

    read_data(fd, 0, (char *) VIRTUAL_ADDRESS_PROGRAM_START,
              MAX_FILE_SIZE);   // read the whole file data (=> offset = 0, length = MAX_FILE_SIZE)

    // close the file
    file_close(&fd);

    // initialize the pcb
    pcb_t *pcb = (pcb_t *) (KERNEL_STACK_BASE_ADDRESS - (3 * USER_KERNEL_STACK_SIZE));
    file_descriptor_table_init(pcb->file_descriptor_array);
    pcb->current_pid = 3;
    pcb->parent_pid = 0;

    pcb->current_esp = USER_STACK_BASE_ADDRESS;
    pcb->current_ebp = USER_STACK_BASE_ADDRESS;
    pcb->current_eip = (*(uint32_t *) (VIRTUAL_ADDRESS_PROGRAM_START + 24));

    pcb->vidmap_flag = 0;

    pcb->terminal = &terminal_3;
    terminal_3.current_process_id = 0;

    memset(pcb->arg_buf, 0, MAX_ARG_LENGTH);

    return;
}

// boot_enter_terminal_1
// Input: none
// Output: none
// Description: enter the first terminal shell
void boot_enter_terminal_1() {
    active_terminal = &terminal_1;
    active_pid = 1;
    terminal_1.current_process_id = 1;

    paging_execute(active_pid);
    paging_video_memory();

    tss.esp0 = KERNEL_STACK_BASE_ADDRESS - (active_pid - 1) * USER_KERNEL_STACK_SIZE - 0x04;
    context_switch_execute();
}

// boot_enter_terminal_2
// Input: none
// Output: none
// Description: enter the second terminal shell
void boot_enter_terminal_2() {
    active_terminal = &terminal_2;
    active_pid = 2;
    terminal_2.current_process_id = 2;

    paging_execute(active_pid);
    paging_video_memory();

    tss.esp0 = KERNEL_STACK_BASE_ADDRESS - (active_pid - 1) * USER_KERNEL_STACK_SIZE - 0x04;
    context_switch_execute();
}

// boot_enter_terminal_3
// Input: none
// Output: none
// Description: enter the third terminal shell
void boot_enter_terminal_3() {
    active_terminal = &terminal_3;
    active_pid = 3;
    terminal_3.current_process_id = 3;

    paging_execute(active_pid);
    paging_video_memory();

    tss.esp0 = KERNEL_STACK_BASE_ADDRESS - (active_pid - 1) * USER_KERNEL_STACK_SIZE - 0x04;
    context_switch_execute();
}

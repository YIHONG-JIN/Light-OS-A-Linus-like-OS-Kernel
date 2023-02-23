//
// Created by JIN01 on 4/10/2022.
//
#include "general_file.h"
#include "filesystem.h"
#include "terminal.h"

extern terminal_t *active_terminal;
extern uint32_t active_pid;

// file_descriptor_table_init
// DESCRIPTION: Initialize the file descriptor table
// INPUTS: pointer to the file descriptor table which is to be initialized
// OUTPUTS: none
int32_t file_descriptor_table_init(file_descriptor_t *fd_table) {
    if (fd_table == NULL) {
        return -1;
    }
    int32_t i;
    for (i = 0; i < MAX_NUM_FILES; i++) {
        if (i == 0) {
            fd_table[i].file_ops_t = &terminal_file_ops_stdin;
            fd_table[i].inode = -1;
            fd_table[i].file_pos = 0;
            fd_table[i].flags = 0;
            continue;
        }
        if (i == 1) {
            fd_table[i].file_ops_t = &terminal_file_ops_stdout;
            fd_table[i].inode = -1;
            fd_table[i].file_pos = 0;
            fd_table[i].flags = 0;
            continue;
        }


        fd_table[i].file_ops_t = NULL;
        fd_table[i].inode = -1;
        fd_table[i].file_pos = 0;
        fd_table[i].flags = 0;
    }
    return 0;
};

// file_descriptor_table_open
// DESCRIPTION: Open a file in the file descriptor table
// INPUTS: pointer to the file descriptor table, file name, flags
// OUTPUTS: file descriptor
int32_t file_descriptor_table_open(file_descriptor_t *fd_table, char *filename) {
    int fd_num = 2;
    while (fd_num < MAX_NUM_FILES && fd_table[fd_num].flags == 1) {
        fd_num++;
    }

    fd_table[fd_num].flags = 1;

    if (fd_num == MAX_NUM_FILES) {
        return -1;
    }

    dir_entry_t dir_entry;
    if (read_dentry_by_name(filename, &dir_entry) != -1) {
        if (dir_entry.filetype == TYPE_FILE) {
            fd_table[fd_num].file_ops_t = &ece391_file_ops;
            fd_table[fd_num].inode = dir_entry.inode_num;
            fd_table[fd_num].file_pos = 0;
        } else if (dir_entry.filetype == TYPE_DIRECTORY) {
            fd_table[fd_num].file_ops_t = &ece391_dir_ops;
            fd_table[fd_num].inode = dir_entry.inode_num;
            fd_table[fd_num].file_pos = 0;
        } else if (dir_entry.filetype == TYPE_RTC) {
            fd_table[fd_num].file_ops_t = &ece391_rtc_ops;
            fd_table[fd_num].inode = dir_entry.inode_num;
            fd_table[fd_num].file_pos = 0;
        }
    } else {
        return -1;
    }
    return fd_num;
};

// file_descriptor_table_close
// DESCRIPTION: Close a file in the file descriptor table
// INPUTS: pointer to the file descriptor table, file descriptor
// OUTPUTS: true if successful, false otherwise
int32_t file_descriptor_table_close(file_descriptor_t *fd_table, int32_t fd) {
    if (fd < 0 || fd >= MAX_NUM_FILES) {
        return -1;
    }
    if (file_close(&fd_table[fd].inode) == -1) {
        return -1;
    }
    fd_table[fd].file_ops_t = NULL;
    fd_table[fd].file_pos = 0;
    fd_table[fd].flags = 0;
    return 0;
};

// file_descriptor_table_read
// DESCRIPTION: Read from a file in the file descriptor table
// INPUTS: pointer to the file descriptor table, file descriptor, buffer, length
// OUTPUTS: number of bytes read if successful, -1 otherwise
int32_t file_descriptor_table_read(file_descriptor_t *fd_table, int32_t fd, int8_t *buf, int32_t length) {
    if (fd < 0 || fd >= MAX_NUM_FILES) {
        return -1;
    }
    if (fd_table[fd].file_ops_t == NULL) {
        return -1;
    }
    if (fd_table[fd].file_ops_t->read == NULL) {
        return -1;
    }
    return fd_table[fd].file_ops_t->read(fd_table[fd].inode, buf, length, &fd_table[fd].file_pos);
};


// file_descriptor_table_write
// DESCRIPTION: Write to a file in the file descriptor table
// INPUTS: pointer to the file descriptor table, file descriptor, buffer, length
// OUTPUTS: number of bytes written if successful, -1 otherwise
int32_t file_descriptor_table_write(file_descriptor_t *fd_table, int32_t fd, int8_t *buf, int32_t length) {
    if (fd < 0 || fd >= MAX_NUM_FILES) {
        return -1;
    }
    if (fd_table[fd].file_ops_t == NULL) {
        return -1;
    }
    if (fd_table[fd].file_ops_t->write == NULL) {
        return -1;
    }
    return fd_table[fd].file_ops_t->write(fd_table[fd].inode, buf, length, &fd_table[fd].file_pos);
};

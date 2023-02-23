//
// Created by JIN01 on 4/10/2022.
//

#ifndef MP3_GENERAL_FILE_H
#define MP3_GENERAL_FILE_H

#include "lib.h"

#define MAX_NUM_FILES 8

typedef struct file_ops {
    int32_t (*open)(int32_t *, char *);

    int32_t (*read)(int32_t, int8_t *, uint32_t, uint32_t *);

    int32_t (*write)(int32_t, int8_t *, uint32_t, uint32_t *);

    int32_t (*close)(int32_t *);
} file_ops_t;

typedef struct file_descriptor {
    file_ops_t *file_ops_t;
    int32_t inode;
    uint32_t file_pos;
    int32_t flags;
} file_descriptor_t;

int32_t file_descriptor_table_init(file_descriptor_t *fd_table);

int32_t file_descriptor_table_open(file_descriptor_t *fd_table, char *filename);

int32_t file_descriptor_table_close(file_descriptor_t *fd_table, int32_t fd);

int32_t file_descriptor_table_read(file_descriptor_t *fd_table, int32_t fd, int8_t *buf, int32_t length);

int32_t file_descriptor_table_write(file_descriptor_t *fd_table, int32_t fd, int8_t *buf, int32_t length);

#endif //MP3_GENERAL_FILE_H

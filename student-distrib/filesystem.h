#ifndef MP3_FILESYSTEM_H
#define MP3_FILESYSTEM_H


#include "lib.h"
#include "general_file.h"


#define FILENAME_LEN 32
#define MAX_DIRECTORY_NUMBER 63
#define BLOCK_SIZE 4096

#define PDE_OFFSET 10
#define PTE_OFFSET 10
#define PAGE_OFFSET 12

#define MAX_FILE_SIZE 63 * 4096

#define TYPE_RTC 0
#define TYPE_DIRECTORY 1
#define TYPE_FILE 2

// directory entry struct
typedef struct dir_entry {
    int8_t filename[FILENAME_LEN];
    uint32_t filetype;
    uint32_t inode_num;
    uint8_t reserved[24];
} dir_entry_t;

// struct for the bootblock
typedef struct boot_block {
    uint32_t dir_count;
    uint32_t inode_count;
    uint32_t data_count;
    uint8_t reserved[52];
    dir_entry_t direntries[MAX_DIRECTORY_NUMBER];
} boot_block_t;

// struct for the index nodes
typedef struct i_node {
    uint32_t length;
    uint32_t data_block_num[1023];
} i_node_t;

// struct for the data blocks
typedef struct data_block {
    uint32_t data[BLOCK_SIZE / 4];
} data_block_t;

void init_filesystem(uint32_t address);

int32_t read_dentry_by_name(const int8_t *fname, dir_entry_t *dentry);

int32_t read_data(int32_t inode, uint32_t offset, int8_t *buf, uint32_t length);

int32_t dir_open(int32_t *fd, char *filename);

int32_t dir_close(int32_t *fd);

int32_t dir_write(int32_t fd, int8_t *buf, uint32_t length, uint32_t *offset);

int32_t dir_read(int32_t fd, int8_t *buf, uint32_t length, uint32_t *offset);

int32_t file_open(int32_t *fd, char *filename);

int32_t file_close(int32_t *fd);

int32_t file_write(int32_t fd, int8_t *buf, uint32_t length, uint32_t *offset);

int32_t file_read(int32_t fd, int8_t *buf, uint32_t length, uint32_t *offset);

extern file_ops_t ece391_file_ops;
extern file_ops_t ece391_dir_ops;
extern file_ops_t ece391_rtc_ops;
boot_block_t *boot_block_pointer;

#endif //MP3_FILESYSTEM_H







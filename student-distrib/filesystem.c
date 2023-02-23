#include "filesystem.h"
#include "rtc.h"

boot_block_t *boot_block_pointer = NULL;

// ece391_file_ops is a global variable that holds the file operations
file_ops_t ece391_file_ops = {
        .open = file_open,
        .read = file_read,
        .write = file_write,
        .close = file_close
};

// ece391_dir_ops is a global variable that holds the directory operations
file_ops_t ece391_dir_ops = {
        .open = dir_open,
        .read = dir_read,
        .write = dir_write,
        .close = dir_close
};

// ece391_fs_ops is a global variable that holds the filesystem operations
file_ops_t ece391_rtc_ops = {
        .open = rtc_open,
        .read = rtc_read,
        .write = rtc_write,
        .close = rtc_close
};

// void init_filesystem(uint32_t address)
// Function: Initialize the filesystem
// Input: uint32_t address - the address of the boot block
// Output: none
// Side effects: sets boot_block_pointer
void init_filesystem(uint32_t address) {
    boot_block_pointer = (boot_block_t *) address;
}


// int32_t read_denry_by_name(const uint8_t* fname, dentry_t* dentry)
// Function: Read entry by name and copy corresponding data to dentry
// Input: const uint8_t* fname - the name of the file
//        dentry_t* dentry - the dentry to be filled
// Output: int32_t - 0 if successful, -1 if not
// Side effects: None
int32_t read_dentry_by_name(const int8_t *fname, dir_entry_t *dentry) {
    if (fname == NULL || dentry == NULL) {
        return -1;
    }

    if (boot_block_pointer == NULL) {
        return -1;
    }

    // Storing lengths of strings
    int length = strlen(fname);

    if (length == 0) {
        return -1;
    }

    int index;
    int entry_len;

    for (index = 0; index < MAX_DIRECTORY_NUMBER; index++) {
        entry_len = strlen(boot_block_pointer->direntries[index].filename);
        if (length == FILENAME_LEN) {
            if (strncmp(fname, boot_block_pointer->direntries[index].filename, FILENAME_LEN) == 0) {
                *dentry = boot_block_pointer->direntries[index];
                return 0;
            }
        }
        // Comparing strings to check
        if (length == entry_len) {
            if (strncmp(fname, boot_block_pointer->direntries[index].filename, length) == 0) {
                *dentry = boot_block_pointer->direntries[index];
                return 0;
            }
        }
    }
    return -1;
}

// int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
// Function: Read data from file
// Input: uint32_t inode - the inode of the file
//        uint32_t offset - the offset of the file
//        uint8_t* buf - the buffer to be filled
//        uint32_t length - the length of the buffer
// Output: int32_t - 0 if successful, -1 if not
// Side effects: None
int32_t read_data(int32_t inode, uint32_t offset, int8_t *buf, uint32_t length) {
    // Handle with edge cases
    uint32_t return_val = 0;
    if (boot_block_pointer == NULL) {
        return -1;
    }
    if (buf == NULL) {
        return -1;
    }
    if (inode >= boot_block_pointer->inode_count || inode < 0) {
        return -1;
    }
    // obtaining the inode
    i_node_t *inode_pointer = (i_node_t *) boot_block_pointer + (1 + inode);;
    if (inode_pointer == NULL) {
        return -1;
    }
    if (offset >= inode_pointer->length || offset < 0) {
        return 0;
    }
    // if length is greater than the length of the file, reduce it
    if (length > inode_pointer->length - offset) {
        length = inode_pointer->length - offset;
    }

    // copying data into buf
    uint32_t first = offset / BLOCK_SIZE;
    uint32_t last = (offset + length) / BLOCK_SIZE;

    uint32_t index;
    for (index = first; index <= last; ++index) {
        uint32_t data_block_number = inode_pointer->data_block_num[index];
        data_block_t *data_block_pointer =
                (data_block_t *) boot_block_pointer + (boot_block_pointer->inode_count + 1 + data_block_number);
        uint32_t block_byte_begin = 0;
        uint32_t block_byte_end = BLOCK_SIZE;
        if (index == first) {
            block_byte_begin = offset % BLOCK_SIZE;
        }
        if (index == last) {
            block_byte_end = (offset + length) % BLOCK_SIZE;
        }
        memcpy((char *) buf, (char *) data_block_pointer + block_byte_begin, block_byte_end - block_byte_begin);
        buf += block_byte_end - block_byte_begin;
        return_val += block_byte_end - block_byte_begin;
    }
    return return_val;
}

// int32_t dir_open(int32_t* fd, char* filename)
// Function: Open a directory
// Input: int32_t* fd - the file descriptor
//        char* filename - the name of the directory
// Output: int32_t - 0 if successful, -1 if not
// Side effects: None
int32_t dir_open(int32_t *fd, char *filename) {
    if ((*fd) != NULL || filename == NULL) {
        return -1;
    }
    if (boot_block_pointer == NULL) {
        return -1;
    }
    dir_entry_t dentry;
    if (!read_dentry_by_name(filename, &dentry)) {
        if (dentry.filetype != TYPE_DIRECTORY) {
            return -1;
        }
        *fd = dentry.inode_num;
        return 0;
    }
    return -1;
};

// int32_t dir_close(int32_t* fd)
// Function: Close a directory
// Input: int32_t* fd - the file descriptor
// Output: int32_t - 0 if successful, -1 if not
// Side effects: None
int32_t dir_close(int32_t *fd) {
    if (boot_block_pointer == NULL) {
        return -1;
    }
    *fd = NULL;
    return 0;
};

// int32_t dir_write(int32_t fd, int8_t* buf, uint32_t length, uint32_t* offset)
// Function: Write to a directory
// Input: int32_t fd - the file descriptor
//        int8_t* buf - the buffer to be written
//        uint32_t length - the length of the buffer
//        uint32_t* offset - the offset of the file
// Output: int32_t - 0 if successful, -1 if not
// Side effects: None
int32_t dir_write(int32_t fd, int8_t *buf, uint32_t length, uint32_t *offset) {
    return -1;
};

// called in a loop to read all the directory entries
// int32_t dir_read(int32_t fd, int8_t* buf, uint32_t length, uint32_t* offset)
// Function: Read from a directory
// Input: int32_t fd - the file descriptor
//        int8_t* buf - the buffer to be filled
//        uint32_t length - the length of the buffer
//        uint32_t* offset - the offset of the file
// Output: int32_t - 0 if successful, -1 if not
// Side effects: None
int32_t dir_read(int32_t fd, int8_t *buf, uint32_t length, uint32_t *offset) {
    // fd is NULL !
    if (fd == -1 || buf == NULL) {
        return -1;
    }
    if (boot_block_pointer == NULL) {
        return -1;
    }
    if (*offset >= boot_block_pointer->inode_count || *offset < 0) {
        return -1;
    }
    memcpy(buf, boot_block_pointer->direntries[*offset].filename, length);
    (*offset)++;
    if (*offset >= 18) {
        return 0;
    }
    return length;
};

// int32_t file_open(int32_t* fd, char* filename)
// Function: Open a file
// Input: int32_t* fd - the file descriptor
//        char* filename - the name of the file
// Output: int32_t - 0 if successful, -1 if not
// Side effects: None
int32_t file_open(int32_t *fd, char *filename) {
    if (filename == NULL) {
        return -1;
    }
    if (boot_block_pointer == NULL) {
        return -1;
    }
    dir_entry_t dentry;
    if (read_dentry_by_name(filename, &dentry) == -1) {
        return -1;
    }
    if (dentry.filetype != TYPE_FILE) {
        return -1;
    }
    *fd = dentry.inode_num;
    return 0;
};

// int32_t file_close(int32_t* fd)
// Function: Close a file
// Input: int32_t* fd - the file descriptor
// Output: int32_t - 0 if successful, -1 if not
// Side effects: None
int32_t file_close(int32_t *fd) {
    if ((*fd) == -1) {
        return -1;
    }
    *fd = -1;
    return 0;
};

// int32_t file_write(int32_t fd, int8_t* buf, uint32_t length, uint32_t* offset)
// Function: Write to a file
// Input: int32_t fd - the file descriptor
//        int8_t* buf - the buffer to be written
//        uint32_t length - the length of the buffer
//        uint32_t* offset - the offset of the file
// Output: int32_t - 0 if successful, -1 if not
// Side effects: None
int32_t file_write(int32_t fd, int8_t *buf, uint32_t length, uint32_t *offset) {
    return -1;
};

// file_read(int32_t fd, int8_t* buf, uint32_t length, uint32_t* offset)
// Function: Read from a file
// Input: int32_t fd - the file descriptor
//        int8_t* buf - the buffer to be filled
//        uint32_t length - the length of the buffer
//        uint32_t* offset - the offset of the file
// Output: int32_t - 0 if successful, -1 if not
// Side effects: None
int32_t file_read(int32_t fd, int8_t *buf, uint32_t length, uint32_t *offset) {
    if (fd == NULL || buf == NULL) {
        return -1;
    }
    if (boot_block_pointer == NULL) {
        return -1;
    }
    int return_value = 0;
    return_value = read_data(fd, *offset, buf, length);
    if (return_value > 0) {
        *offset += length;
        return return_value;
    }
    return return_value;
};






















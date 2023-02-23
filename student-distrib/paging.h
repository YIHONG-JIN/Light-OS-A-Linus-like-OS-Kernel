
//#pragma once
#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"
#include "lib.h"

#define SIZE 1024
#define ALIGNER 4096
#define VIDEO_ADDRESS_TO_PHYSICAL 0x000B7000
#define VIDEO_MEM 0x000B8000
#define VIDEO_MEM_TERMINAL_1 0x000B9000
#define VIDEO_MEM_TERMINAL_2 0x000BA000
#define VIDEO_MEM_TERMINAL_3 0x000BB000


typedef struct __attribute__ ((packed)) {
    // struct for page directories
    uint32_t present: 1;
    uint32_t write_read: 1;
    uint32_t u_s: 1;
    uint32_t write_through: 1;
    uint32_t cache_disable: 1;
    uint32_t access: 1;
    uint32_t dirty: 1;
    uint32_t page_size: 1;
    uint32_t glob: 1;
    uint32_t avail: 3;
    uint32_t address: 20;

} pde;

typedef struct __attribute__ ((packed)) {
    // struct for page table
    uint32_t present: 1;
    uint32_t write_read: 1;
    uint32_t u_s: 1;
    uint32_t write_through: 1;
    uint32_t cache_disable: 1;
    uint32_t access: 1;
    uint32_t dirty: 1;
    uint32_t pat: 1;
    uint32_t glob: 1;
    uint32_t avail: 3;
    uint32_t address: 20;

} pte;

extern pde page_dir[SIZE];
extern pte pg_tab_1[SIZE];
extern pte vidmap_pg_tab_1[SIZE];


#ifndef ASM

extern void paging_enable(pde *page_dir);

#endif

void page_initialize();

#endif

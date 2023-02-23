
#include "paging.h"

pde page_dir[SIZE] __attribute__((aligned (ALIGNER)));
pte pg_tab_1[SIZE] __attribute__((aligned (ALIGNER)));
pte vidmap_pg_tab_1[SIZE] __attribute__((aligned (ALIGNER)));

// void page_initialize()
// Input: none
// Output: none
// This function is used to initialize paging for x86
void page_initialize() {

    int i;
    // first 4
    // setting the control bits to appropriate values
    page_dir[0].present = 1;
    page_dir[0].write_read = 1;
    page_dir[0].u_s = 0;
    page_dir[0].write_through = 0;
    page_dir[0].cache_disable = 0;
    page_dir[0].access = 0;
    page_dir[0].dirty = 0;
    page_dir[0].page_size = 0;
    page_dir[0].glob = 0;
    page_dir[0].avail = 0;
    page_dir[0].address = (uint32_t) pg_tab_1 >> 12;

    // second 4 mb -> kernel
    page_dir[1].present = 1;
    page_dir[1].write_read = 1;
    page_dir[1].u_s = 0;
    page_dir[1].write_through = 0;
    page_dir[1].cache_disable = 1;
    page_dir[1].access = 0;
    page_dir[1].dirty = 0;
    page_dir[1].page_size = 1;
    page_dir[1].glob = 1;
    page_dir[1].avail = 0;
    page_dir[1].address = 0x00400; // this sets the page frame address to the second 4mb page

    // rest is not present
    for (i = 2; i < SIZE; i++) {
        page_dir[i].present = 0;
        page_dir[i].write_read = 0;
        page_dir[i].u_s = 0;
        page_dir[i].write_through = 0;
        page_dir[i].cache_disable = 0;
        page_dir[i].access = 0;
        page_dir[i].dirty = 0;
        page_dir[i].page_size = 0;
        page_dir[i].glob = 0;
        page_dir[i].avail = 0;
        page_dir[i].address = 0;

    }

    for (i = 0; i < SIZE; i++) {
        pg_tab_1[i].present = 0; // set the first page table to not present
        pg_tab_1[i].write_read = 0;
        pg_tab_1[i].u_s = 0;
        pg_tab_1[i].write_through = 0;
        pg_tab_1[i].cache_disable = 0;
        pg_tab_1[i].access = 0;
        pg_tab_1[i].dirty = 0;
        pg_tab_1[i].pat = 0;
        pg_tab_1[i].glob = 0;
        pg_tab_1[i].avail = 0;
        pg_tab_1[i].address = 0;

        vidmap_pg_tab_1[i].present = 0; // set the first page table to not present
        vidmap_pg_tab_1[i].write_read = 0;
        vidmap_pg_tab_1[i].u_s = 0;
        vidmap_pg_tab_1[i].write_through = 0;
        vidmap_pg_tab_1[i].cache_disable = 0;
        vidmap_pg_tab_1[i].access = 0;
        vidmap_pg_tab_1[i].dirty = 0;
        vidmap_pg_tab_1[i].pat = 0;
        vidmap_pg_tab_1[i].glob = 0;
        vidmap_pg_tab_1[i].avail = 0;
        vidmap_pg_tab_1[i].address = 0;
    }
    pg_tab_1[(VIDEO_ADDRESS_TO_PHYSICAL) >> 12].present = 1;
    pg_tab_1[(VIDEO_ADDRESS_TO_PHYSICAL) >> 12].write_read = 1;
    pg_tab_1[(VIDEO_ADDRESS_TO_PHYSICAL) >> 12].u_s = 0;
    pg_tab_1[(VIDEO_ADDRESS_TO_PHYSICAL) >> 12].write_through = 0;
    pg_tab_1[(VIDEO_ADDRESS_TO_PHYSICAL) >> 12].cache_disable = 0;
    pg_tab_1[(VIDEO_ADDRESS_TO_PHYSICAL) >> 12].access = 0;
    pg_tab_1[(VIDEO_ADDRESS_TO_PHYSICAL) >> 12].dirty = 0;
    pg_tab_1[(VIDEO_ADDRESS_TO_PHYSICAL) >> 12].pat = 0;
    pg_tab_1[(VIDEO_ADDRESS_TO_PHYSICAL) >> 12].glob = 0;
    pg_tab_1[(VIDEO_ADDRESS_TO_PHYSICAL) >> 12].avail = 0;
    pg_tab_1[(VIDEO_ADDRESS_TO_PHYSICAL) >> 12].address = VIDEO_MEM >> 12;

    // bit shift to get the appropriate addresss to access video memory portion
    pg_tab_1[(VIDEO_MEM) >> 12].present = 1;
    pg_tab_1[(VIDEO_MEM) >> 12].write_read = 1;
    pg_tab_1[(VIDEO_MEM) >> 12].u_s = 0;
    pg_tab_1[(VIDEO_MEM) >> 12].write_through = 0;
    pg_tab_1[(VIDEO_MEM) >> 12].cache_disable = 0;
    pg_tab_1[(VIDEO_MEM) >> 12].access = 0;
    pg_tab_1[(VIDEO_MEM) >> 12].dirty = 0;
    pg_tab_1[(VIDEO_MEM) >> 12].pat = 0;
    pg_tab_1[(VIDEO_MEM) >> 12].glob = 0;
    pg_tab_1[(VIDEO_MEM) >> 12].avail = 0;
    pg_tab_1[(VIDEO_MEM) >> 12].address = VIDEO_MEM >> 12;

    // bit shift to get the appropriate addresss to access video memory portion
    pg_tab_1[(VIDEO_MEM_TERMINAL_1) >> 12].present = 1;
    pg_tab_1[(VIDEO_MEM_TERMINAL_1) >> 12].write_read = 1;
    pg_tab_1[(VIDEO_MEM_TERMINAL_1) >> 12].u_s = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_1) >> 12].write_through = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_1) >> 12].cache_disable = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_1) >> 12].access = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_1) >> 12].dirty = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_1) >> 12].pat = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_1) >> 12].glob = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_1) >> 12].avail = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_1) >> 12].address = VIDEO_MEM_TERMINAL_1 >> 12;

    // bit shift to get the appropriate addresss to access video memory portion
    pg_tab_1[(VIDEO_MEM_TERMINAL_2) >> 12].present = 1;
    pg_tab_1[(VIDEO_MEM_TERMINAL_2) >> 12].write_read = 1;
    pg_tab_1[(VIDEO_MEM_TERMINAL_2) >> 12].u_s = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_2) >> 12].write_through = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_2) >> 12].cache_disable = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_2) >> 12].access = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_2) >> 12].dirty = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_2) >> 12].pat = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_2) >> 12].glob = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_2) >> 12].avail = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_2) >> 12].address = VIDEO_MEM_TERMINAL_2 >> 12;

    // bit shift to get the appropriate addresss to access video memory portion
    pg_tab_1[(VIDEO_MEM_TERMINAL_3) >> 12].present = 1;
    pg_tab_1[(VIDEO_MEM_TERMINAL_3) >> 12].write_read = 1;
    pg_tab_1[(VIDEO_MEM_TERMINAL_3) >> 12].u_s = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_3) >> 12].write_through = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_3) >> 12].cache_disable = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_3) >> 12].access = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_3) >> 12].dirty = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_3) >> 12].pat = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_3) >> 12].glob = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_3) >> 12].avail = 0;
    pg_tab_1[(VIDEO_MEM_TERMINAL_3) >> 12].address = VIDEO_MEM_TERMINAL_3 >> 12;
    paging_enable(page_dir);

}



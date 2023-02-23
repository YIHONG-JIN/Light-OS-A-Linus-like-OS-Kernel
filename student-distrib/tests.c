#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "filesystem.h"
#include "terminal.h"
#include "rtc.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER    \
    printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)    \
    printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure() {
    /* Use exception #15 for assertions, otherwise
       reserved by Intel */
    asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
//int idt_test(){
//	TEST_HEADER;
//
//	int i;
//	int result = PASS;
//	for (i = 0; i < 10; ++i){
//		if ((idt[i].offset_15_00 == NULL) &&
//			(idt[i].offset_31_16 == NULL)){
//			assertion_failure();
//			result = FAIL;
//		}
//	}
//
//	return result;
//}
//
//int exception_test() {
//    TEST_HEADER;
//
//    printf("Zero division exception:\n");
//
//    int a = 1;
//    int b = 0;
//    a = a / b;
//
//    return FAIL;	// should never reach this
//}
//
// add more tests here
//
//
//
//int null_ptr_page() {
//	TEST_HEADER;
//	char* null_ptr;
//	char checker;
//	null_ptr = (char*)0;
//	checker = *null_ptr;
//
//	return FAIL;
//}
//
//int ptr_8mb_plus() {
//	TEST_HEADER;
//	int val;
//	char* ptr_above_8mb;
//	char checker;
//	val = 0x0800000;
//	val = val + 0x00010;
//	ptr_above_8mb = (char*)val;
//	checker = *ptr_above_8mb;
//
//	return FAIL;
//}
//
//int ptr_in_vid() {
//	TEST_HEADER;
//	int val;
//	char* ptr_in_4kb;
//	char checker;
//	val = 0x000B8000;
//	val = val + 0x00010;
//	ptr_in_4kb = (char*)val;
//	checker = *ptr_in_4kb;
//
//	return PASS;
//}
//
/* Checkpoint 2 tests */

//int read_existent_file() {
//    TEST_HEADER;
//    dir_entry_t file_descriptor;
//    if(-1 == read_dentry_by_name("frame0.txt", &file_descriptor)) return FAIL;
//    char buf[200];
//    if(-1 == read_data(file_descriptor.inode_num, 0, buf, 174)) return FAIL;
//    int i;
//    for(i = 0; i < 174; i++) putc(buf[i]);
//
//    printf("\n");
//    return PASS;
//}
//
//int read_nonexistent_file() {
//    TEST_HEADER;
//    dir_entry_t file_descriptor;
//    if(-1 != read_dentry_by_name("fake.txt", &file_descriptor)) return FAIL;
//    return PASS;
//}
//
//int read_existent_file_general() {
//    TEST_HEADER;
//    int32_t fd = NULL;
//    uint32_t offset = 0;
//    char buf[200];
//    if (-1 == file_open(&fd, "frame0.txt")) return FAIL;
//    if (-1 == file_read(fd, buf, 200, &offset)) return FAIL;
//
//    printf("%s\n", buf);
//
//    if (-1 == file_close(&fd)) return FAIL;
//
//    return PASS;
//}
//
//int read_nonexistent_file_general() {
//    TEST_HEADER;
//    int32_t fd = NULL;
//    uint32_t offset = 0;
//    char buf[200];
//    if (-1 == file_open(&fd, "fake.txt")) return FAIL;
//    if (-1 != file_read(fd, buf, 200, &offset)) return FAIL;
//
//    if (-1 == file_close(&fd)) return FAIL;
//
//    return PASS;
//}
//
//#define NUMBER_OF_DIRECTORY_ENTRIES 17
//int read_existent_directory() {
//    TEST_HEADER;
//    int32_t fd = NULL;
//    char buf[FILENAME_LEN];
//    if (-1 == dir_open(&fd, ".")) return FAIL;
//
//    int32_t result;
//    uint32_t offset = 0;
//    while (1) {
//        result = dir_read(fd, buf, FILENAME_LEN, &offset);
//        if (result == -1) return FAIL;
//        printf("%s\n", buf);
//        if (offset == MAX_DIRECTORY_NUMBER - 1) break;
//        if (offset == NUMBER_OF_DIRECTORY_ENTRIES) break;
//    }
//    if (-1 == dir_close(&fd)) return FAIL;
//
//    return PASS;
//}
//
//int read_nonexistent_directory() {
//    TEST_HEADER;
//    int32_t fd = NULL;
//    char buf[FILENAME_LEN];
//    if (-1 == dir_open(&fd, "fake")) return FAIL;
//
//    int32_t result;
//    uint32_t offset = 0;
//    while (1) {
//        result = dir_read(fd, buf, FILENAME_LEN, &offset);
//        if (result == -1) return FAIL;
//        printf("%s\n", buf);
//        if (offset == MAX_DIRECTORY_NUMBER - 1) break;
//        if (offset == NUMBER_OF_DIRECTORY_ENTRIES) break;
//    }
//    if (-1 == dir_close(&fd)) return FAIL;
//
//    return PASS;
//}

//int terminal_driver_test()
//{
//    TEST_HEADER;
//    int32_t read_retval, write_retval;
//    terminal_open(NULL);
//    char buf[KEYBOARD_BUFFER_SIZE];
//    printf("Input: ");
//    read_retval = terminal_read(NULL, buf, 128);
//    printf("output: ");
//    write_retval = terminal_write(NULL, buf, 128);
//    terminal_close(NULL);
//    printf("Read %d characters from terminal, and write %d characters to terminal.\n",
//           read_retval, write_retval);
//    return PASS;
//}

/*
int rtc_write_test() {
    TEST_HEADER;
    rtc_open(NULL);
    int8_t rate = 5;
    rtc_write(NULL, &rate, 0);
    rtc_close(NULL);
    return PASS;
}
*/


//davenport's version. only tests write at each rate once, results don't look very intuitive so i'll admit its a pretty dooky test
//int rtc_test() {
//    //TEST_HEADER;
//    printf("opening RTC test and running it... now!\n");
//    rtc_open(0,NULL);
//    int32_t rate = 2;
//    int32_t i;
//
//    //this block tests that frequency properly sets all the way through 1024
//    while (rate < 1030) {
//        printf("Testing rate %d: \n", rate);
//        for (i = 0; i < rate; i++) {
//            if (rtc_write(NULL, &rate, 0, 0) == 0) putc('1');
//            else putc('0');
//            rtc_read(NULL, &rate, 0, 0);
//        }
//        putc('\n');
//        rate = rate << 1;
//    }
//    rtc_close(NULL);
//    return PASS;
//}
//
//
//int rtc_read_test() {
//    TEST_HEADER;
//    rtc_open(0,NULL);
//    int input = 0;
//    while (1) {
//        input = inb(KEYBOARD_PORT);
//        if (input == 0x1C) {
//            break;
//        }
//    }
//    printf("\nnew RTC rate: %d\n", 14);
//
//    int8_t rate = 14;
//    rtc_write(NULL, &rate, 0, 0);
//    rtc_read(NULL, &rate, 0, 0);
//    rtc_close(NULL);
//    return PASS;
//}



/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests() {
    //TEST_OUTPUT("idt_test", idt_test());
    //TEST_OUTPUT("exception_test", exception_test());
    // launch your tests here

    /* PAGING TESTS HERE */
//    TEST_OUTPUT("ptr in first 4kb in vid test", ptr_in_vid());
//	  TEST_OUTPUT("null ptr test", null_ptr_page());
//	  TEST_OUTPUT("ptr beyond 8mb test", ptr_8mb_plus());
    /* END PAGING TESTS */

    /* FILE SYSTEM TESTS HERE */
//    TEST_OUTPUT("read existent file general test", read_existent_file_general());
//    TEST_OUTPUT("read nonexistent file general test", read_nonexistent_file_general());

//    TEST_OUTPUT("read existent directory", read_existent_directory());
//    TEST_OUTPUT("read nonexistent directory", read_nonexistent_directory());

    /* END FILE SYSTEM TESTS */

    /* TERMINAL TESTS HERE */
    // please comment out RTC init
//    TEST_OUTPUT("Terminal Driver Write Test", terminal_driver_test());
    /* END TERMINAL TESTS */

    /* RTC TESTS HERE */
//    TEST_OUTPUT("RTC Driver Write Test", rtc_write_test());
//    TEST_OUTPUT("RTC Driver Read Test", rtc_read_test());
//      TEST_OUTPUT("RTC Driver Test", rtc_test());
    /* END RTC TESTS */


}

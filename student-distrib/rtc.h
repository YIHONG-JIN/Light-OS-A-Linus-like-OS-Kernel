//
// Created by JIN01 on 3/19/2022.
//

#ifndef MP3_RTC_H
#define MP3_RTC_H

#include "lib.h"
#include "i8259.h"
#include "idtInit.h"
#include "syscall.h"
//#include "filesystem.h"

#define REGISTER_A  0x8A
#define REGISTER_B  0x8B
#define REGISTER_C  0x8C

#define RTC_PORT_0x70   0x70
#define RTC_PORT_0x71   0x71

//functions that affect the physical RTC device
void rtc_enable_pie();

void rtc_disable_pie();

void rtc_set_interrupt_rate(uint8_t rate);

int32_t rtc_open(int32_t *fd, char *filename);

int32_t rtc_close(int32_t *fd);

int32_t rtc_read_device(int32_t fd, void *buf, int32_t nbytes);

int32_t rtc_write_device(int32_t fd, const void *buf, int32_t nbytes);

//functions that change/use the virtualized RTC
void rtc_handler();

int32_t rtc_read(int32_t fd, int8_t *buf, uint32_t nbytes, uint32_t *);

int32_t rtc_write(int32_t fd, int8_t *buf, uint32_t nbytes, uint32_t *);

#endif //MP3_RTC_H

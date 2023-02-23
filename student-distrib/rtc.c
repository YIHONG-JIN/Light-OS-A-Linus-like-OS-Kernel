//
// Created by JIN01 on 3/19/2022.
//
// A typical OS will use the APIC or PIT for timing purposes. However, the RTC works just as well. RTC stands for Real Time Clock. It is the chip that keeps your computer's clock up-to-date. Within the chip is also the 64 bytes of CMOS RAM.
// If you simply want information about reading the date/time from the RTC, then please see the CMOS article. The rest of this article covers the use of RTC interrupts.
// This driver is written by referencing from https://wiki.osdev.org/RTC

#include "rtc.h"
#include "filesystem.h"

static volatile int8_t rtc_read_flag = 0;
// static volatile int32_t virtual_rate = 2;
static volatile int32_t physical_rate = 1024;


// void rtc_handler()
// input: none
// output: none
// side effects: sets the read flag to 0
// functionality: this function will be called if a rtc signal is received
void rtc_handler() {
    rtc_read_flag = 0;

    // even if you don't care about what type of interrupt it is,
    // just attach this code to the bottom of your IRQ handler to be sure you get another interrupt.
    // select register C
    outb(REGISTER_C, RTC_PORT_0x70);
    // just throw away contents
    inb(RTC_PORT_0x71);

    if (terminal_1.virtual_count > 0) {
        terminal_1.virtual_count--;
    }

//    if (terminal_1.virtual_count == -1)
//        terminal_1.virtual_count = physical_rate / terminal_1.virtual_rate;

    if (terminal_2.virtual_count > 0) {
        terminal_2.virtual_count--;
    }
//    if (terminal_2.virtual_count == -1)
//        terminal_2.virtual_count = physical_rate / terminal_2.virtual_rate;

    if (terminal_3.virtual_count > 0) {
        terminal_3.virtual_count--;
    }
//    if (terminal_3.virtual_count == -1)
//        terminal_3.virtual_count = physical_rate / terminal_3.virtual_rate;

    send_eoi(IRQ_NUMBER_REAL_TIME_CLOCK);
}

// void rtc_enable_pie()
// input: none
// output: none
// functionality: sets the Periodic Interrupt Enable bit, bit 6 of register B, to 1
void rtc_enable_pie() {
    // set index to register B
    outb(REGISTER_B, RTC_PORT_0x70);
    // get initial value of register B
    char prev = inb(RTC_PORT_0x71);
    // reset index to B
    outb(REGISTER_B, RTC_PORT_0x70);
    //set the bit 6 of register B, turning on periodic interrupts
    outb(prev | 0x40, RTC_PORT_0x71);
}

// void rtc_disable_pie()
// input: none
// output: none
// functionality: sets the Periodic Interrupt Enable bit, bit 6 of register B, to 0
void rtc_disable_pie() {
    // set index to register B
    outb(REGISTER_B, RTC_PORT_0x70);
    // get initial value of register B
    char prev = inb(RTC_PORT_0x71);
    // reset index to B
    outb(REGISTER_B, RTC_PORT_0x70);
    //set the bit 6 of register B, turning on periodic interrupts
    outb(prev & 0xBF, RTC_PORT_0x71);
}

// void rtc_set_interrupt_rate(uint8_t)
// input: rate - A one-byte value whose bottom four bits contain the desired frequency code to be sent to RTC's register A
// output: none
// functionality: this function will change the periodic interrupt_rate
void rtc_set_interrupt_rate(uint8_t rate) {
    // set index to register A, disable NMI
    outb(REGISTER_A, RTC_PORT_0x70);
    // get initial value of register A
    char prev = inb(RTC_PORT_0x71);
    // reset index to A
    outb(REGISTER_A, RTC_PORT_0x70);
    outb((prev & 0xF0) | rate, RTC_PORT_0x71);
}

// void rtc_open(const uint8_t*)
// input: filename - not used, should be NULL
// output: 0
// functionality: this function will initialize the physical and virtual RTC device
int32_t rtc_open(int32_t *fd, char *filename) {
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
    rtc_enable_pie();
    physical_rate = 1024;
    rtc_set_interrupt_rate(0x06); //set physical device to highest frequency, 1024 Hz
    // virtual_rate = 2; //this sets the rate of the virtual device to 2 Hz
    active_terminal->virtual_rate = 2;
    enable_irq(IRQ_NUMBER_REAL_TIME_CLOCK);
    return 0;
}

// void rtc_close(int32_t)
// input: fd - not used
// output: 0
// functionality: this function closes the RTC device
int32_t rtc_close(int32_t *fd) {
    rtc_disable_pie();
    return 0;
}

// void rtc_read(int32_t, void*, int32_t)
// input: fd - not used
//        buf - not used, should be NULL
//        nbytes - not used
// output: 0 upon success, -1 upon failure
// functionality: this function will wait for a calculated number of actual interrupts to 
//                occur before sending a virtualized interrupt.
//                USED TO READ FROM THE VIRTUALZIED RTC DEVICE, NOT PHYSICAL
int32_t rtc_read(int32_t fd, int8_t *buf, uint32_t nbytes, uint32_t *offset) {
    // //x = number of physical interrupts that must occur before a virtual 'interrupt' occurs
    // int32_t x = physical_rate/virtual_rate; 
    // int32_t count = 0;

    // while(count < x) {
    //     rtc_read_device(fd, buf, nbytes);
    //     count = count + 1;
    // }

    active_terminal->virtual_count = physical_rate / active_terminal->virtual_rate;

    while (active_terminal->virtual_count != 0);

    //exits successfully after the device has read been x times
    return 0;
}

// void rtc_read_device(int32_t, void*, int32_t)
// input: fd - not used
//        buf - not used, should be NULL
//        nbyteskn success, -1 upon failure
// functionality: this function will poll in a loop until an interrupt is processed
//                USED TO READ FROM THE PHYSICAL RTC DEVICE, NOT VIRTUALIZED
int32_t rtc_read_device(int32_t fd, void *buf, int32_t nbytes) {
    //set high then wait for an interrupt to occur
    rtc_read_flag = 1;

    //setup an infinite loop that returns 0 once the interrupt occurred.
    while (1) {
        if (rtc_read_flag == 0) {
            return 0;
        }
    }
    return -1; //won't reach this, but just in case
}

// void rtc_write(int32_t, const void*, int32_t)
// input: fd - not used
//        buf - pointer to an int32_t value that specifies the desired RTC frequency in Hz 
//        nbytes - not used
// output: 0 upon success, -1 upon failure
// functionality: ensures the desired frequency is a power of 2 and in bounds then sets the 
//                virtualized RTC frequency to that value
//                USED TO WRITE TO THE VIRTUALIZED RTC DEVICE, NOT PHYSICAL
int32_t rtc_write(int32_t fd, int8_t *buf, uint32_t nbytes, uint32_t *offset) {
    //invalid pointer
    if (buf == NULL) return -1;
    //rate is out of bounds
    int32_t rate = *(int32_t *) buf;
    if (rate < 2 || rate > 1024) { // 2 <= rate <= 1024
        return -1;
    }

    int32_t i = 2;
    //Check all powers of 2 until 1024. Once we have a match set the virtual rate to this rate then return success
    while (i < 1030) {
        if (i == rate) {
            // virtual_rate = rate;
            active_terminal->virtual_rate = rate;
            active_terminal->virtual_count = physical_rate / active_terminal->virtual_rate;
            return 0;
        } else i = i << 1; //i = i*2
    }
    //rate isn't a power of 2
    return -1;
}

// void rtc_write_device(int32_t, const void*, int32_t)
// input: fd - not used
//        buf - pointer to an int32_t value that specifies the desired RTC frequency in Hz 
//        nbytes - not used
// output: 0 upon success, -1 upon failure
// functionality: ensures the desired frequency is a power of 2 and in bounds then sets 
//                the physical RTC frequency to that value
//                USED TO WRITE TO THE PHYSICAL RTC DEVICE, NOT VIRTUALIZED
int32_t rtc_write_device(int32_t fd, const void *buf, int32_t nbytes) {
    //invalid pointer
    if (buf == NULL) {
        return -1;
    }

    //rate is out of bounds
    int32_t rate = *(int32_t *) buf;
    if (rate < 2 || rate > 1024) { // 2 <= rate <= 1024
        return -1;
    }

    switch (rate) {
        case 2:
            rtc_set_interrupt_rate(0x0F);
            physical_rate = 2;
            return 0;
            break;
        case 4:
            rtc_set_interrupt_rate(0x0E);
            physical_rate = 4;
            return 0;
            break;
        case 8:
            rtc_set_interrupt_rate(0x0D);
            physical_rate = 8;
            return 0;
            break;
        case 16:
            rtc_set_interrupt_rate(0x0C);
            physical_rate = 16;
            return 0;
            break;
        case 32:
            rtc_set_interrupt_rate(0x0B);
            physical_rate = 32;
            return 0;
            break;
        case 64:
            rtc_set_interrupt_rate(0x0A);
            physical_rate = 64;
            return 0;
            break;
        case 128:
            rtc_set_interrupt_rate(0x09);
            physical_rate = 128;
            return 0;
            break;
        case 256:
            rtc_set_interrupt_rate(0x08);
            physical_rate = 256;
            return 0;
            break;
        case 512:
            rtc_set_interrupt_rate(0x07);
            physical_rate = 512;
            return 0;
            break;
        case 1024:
            rtc_set_interrupt_rate(0x06);
            physical_rate = 1024;
            return 0;
            break;
        default: //rate isn't a power of 2
            return -1;
    }
}

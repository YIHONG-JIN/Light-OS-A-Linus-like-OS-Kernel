/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
// Input: none
// Output: none
// Description: initializes both primary and secondary PIC
void i8259_init(void) {
    // ICW1: select 8259A-1 init
    outb(ICW1, MASTER_8259_PORT);
    // ICW1: select 8259A-2 init
    outb(ICW1, SLAVE_8259_PORT);

    // ICW2: 8259A-1 IRO-7 mapped to 0x20-0x27
    outb(ICW2_MASTER, PORT_MASTER);
    // ICW2: 8259A-2 IRO-7 mapped to 0x28-0x2f
    outb(ICW2_SLAVE, PORT_SLAVE);

    // 8259A-1 (the primary) has a secondary on IRQ2
    outb(ICW3_MASTER, PORT_MASTER);
    // 8259A-2 is a secondary on primary's IRQ2
    outb(ICW3_SLAVE, PORT_SLAVE);

    outb(ICW4, PORT_MASTER);
    outb(ICW4, PORT_SLAVE);

    // mask both PICs
    outb(MASK_ALL, PORT_MASTER);
    outb(MASK_ALL, PORT_SLAVE);

    enable_irq(IRQ_NUMBER_OF_SLAVE);
}

/* Enable (unmask) the specified IRQ */
// Input: irq_num (number of interrupt request to unmask)
// Output: none
// Description: clears mask of the specified interrupt request
void enable_irq(uint32_t irq_num) {
    if (irq_num < 0 || irq_num > MAXIMUM_IRQ_NUMBER) {      // check the boundaries
        return;
    }

    // The code below was written using reference from https://wiki.osdev.org/8259_PIC (IRQ_clear_mask)

    uint16_t port;
    uint8_t value;

    if (irq_num < NUMBER_IRQ_MASTER) {            // determine which PIC to use
        port = PORT_MASTER;
    } else {
        port = PORT_SLAVE;
        irq_num -= NUMBER_IRQ_MASTER;
    }

    value = inb(port) & ~(1 << irq_num);          // calculate the value to write
    outb(value, port);
}

/* Disable (mask) the specified IRQ */
// Input: irq_num (number of interrupt request to mask)
// Output: none
// Description: sets mask of the specified interrupt request
void disable_irq(uint32_t irq_num) {
    if (irq_num < 0 || irq_num > MAXIMUM_IRQ_NUMBER) {            // check the boundaries
        return;
    }

    // The code below was written using reference from https://wiki.osdev.org/8259_PIC (IRQ_set_mask)

    uint16_t port;
    uint8_t value;

    if (irq_num < NUMBER_IRQ_MASTER) {            // determine which PIC to use
        port = PORT_MASTER;
    } else {
        port = PORT_SLAVE;
        irq_num -= NUMBER_IRQ_MASTER;
    }

    value = inb(port) | (1 << irq_num);           // calculate the value to write
    outb(value, port);
}

/* Send end-of-interrupt signal for the specified IRQ */
// Input: irq_num (number of the interrupt request)
// Output: none
// Description: sends eoi signaling that interrupt request has been serviced
void send_eoi(uint32_t irq_num) {
    if (irq_num < 0 || irq_num > MAXIMUM_IRQ_NUMBER) {        // check the boundaries
        return;
    }

    uint16_t port;
    uint8_t value;

    if (irq_num >= NUMBER_IRQ_MASTER) {              // depending on PIC choose the right port to write to
        port = PORT_SLAVE;
        irq_num -= NUMBER_IRQ_MASTER;
        value = EOI | irq_num;
        outb(value, SLAVE_8259_PORT);
        send_eoi(IRQ_NUMBER_OF_SLAVE);
        return;
    }
    value = EOI | irq_num;
    outb(value, MASTER_8259_PORT);
    return;
}

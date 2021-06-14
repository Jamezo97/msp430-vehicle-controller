/*
 * ucs.inl.cpp
 *
 *  Created on: 6 Jun 2021
 *      Author: James Wyatt
 */

// All common code between UART and SPI can go here!

#include <msp430.h>

#define IS_TX_BUF_READY (UC0IFG & UCxxTXIFG)
#define IS_RX_BUF_READY (UC0IFG & UCxxRXIFG)
#define ENABLE_TX_IR    (IE2 |= UCxxTXIE)
#define DISABLE_TX_IR   (IE2 &= ~UCxxTXIE)
#define CHECK_TX_IR     (IE2 & UCxxTXIE)
#define ENABLE_RX_IR    (IE2 |= UCxxRXIE)
#define DISABLE_RX_IR   (IE2 &= ~UCxxRXIE)
#define CHECK_RX_IR     (IE2 & UCxxRXIE)

ShiftRegister<uint8_t, TX_BUFF_SIZE, uint8_t> tx_buffer;
ShiftRegister<uint8_t, RX_BUFF_SIZE, uint8_t> rx_buffer;
// Clear these at the start of an SPI transmission
volatile unsigned int tx_counter = 0;
volatile unsigned int rx_counter = 0;

void transmit(const uint8_t* data, unsigned int count) {
    __disable_interrupt(); // prevent IR from breaking the following logic, we'll enable IR when we sleep
    ENABLE_TX_IR; // enable TX ISR, either to notify us when empty or to cleanup when done
    while(count--) {
        while(!tx_buffer.shift(*data)) {
            // Go to sleep, wake up when UART TX is triggered
            ENABLE_TX_IR; // specifically enable again?
            __wakeup_on(WAKEUP_xx_TX);
            GO_TO_SLEEP;
        }
        tx_counter++;
        data++; // next byte
    }
    __enable_interrupt(); // all done, enable interrupts (outside of LPM) again
}

void transmit(uint8_t value) {
    transmit(&value, 1);
}

void receive(uint8_t* data, unsigned int count) {
    __disable_interrupt();
//    ENABLE_RX_IR; // this should be enabled anyway??
    while(count--) {
        while(!rx_buffer.unshift(*data)) {
            __wakeup_on(WAKEUP_xx_RX); // wake me up when there's something in the buffer
            GO_TO_SLEEP;
        }
        data++; // next byte
    }
    __enable_interrupt(); // all done, enable interrupts (outside of LPM) again
}

uint8_t receive() {
    uint8_t result;
    receive(&result, 1);
    return result;
}

uint8_t countrx() {
    return rx_buffer.size();
}

uint8_t counttx() {
    return tx_buffer.size();
}

void clear_rx() {
    __disable_interrupt();
    rx_buffer.clear();
    __enable_interrupt();
}

void clear_tx() {
    __disable_interrupt();
    tx_buffer.clear();
    __enable_interrupt();
}

void delay_cycles(unsigned int count) {
    while(count--) {
        __delay_cycles(CLOCK_DIV);
    }
}


bool read(uint8_t& result) {
    return rx_buffer.unshift(result);
}

void flush_tx() {
    if(!CHECK_TX_IR) {
        return;
    }
    __disable_interrupt();
    while(CHECK_TX_IR) {
        ENABLE_TX_IR;
        __wakeup_on(WAKEUP_xx_TX);
        GO_TO_SLEEP;
    }
    __enable_interrupt();
}


/**
 * This is called when the TX buffer is ready to accept a new byte
 * So, if there are bytes in the TX buffer, then send those
 * Otherwise, there's nothing to do, disable the TX ISR
 */
void OnTxISR() {
    uint8_t result;
    // Don't unshift straight into volatile register, does some funky stuff
    if(tx_buffer.unshift(result)) {
        // New byte to transmit
        UCxxTXBUF = result;
    }
    if(tx_buffer.empty()){
        // All done, stop interrupts
        DISABLE_TX_IR;
    }
}

void OnRxISR() {
    // Chuck it into our byte shift register
    uint8_t result = UCxxRXBUF; // reading this register will reset the RX ISR flag
    rx_buffer.shift(result);
    rx_counter++;
}

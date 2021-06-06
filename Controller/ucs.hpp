/*
 * ucs.inl.cpp
 *
 *  Created on: 6 Jun 2021
 *      Author: James Wyatt
 */

// All common code between UART and SPI can go here!

#include <msp430.h>

#ifndef INLINE
#define INLINE
#endif

ShiftRegister<uint8_t, TX_BUFF_SIZE, uint8_t> tx_buffer;
ShiftRegister<uint8_t, RX_BUFF_SIZE, uint8_t> rx_buffer;
uint8_t flushing = 0;

INLINE void transmit(uint8_t value) {
    if(IS_TX_BUF_READY) {
        // In theory because we're interrupt driven, buffer should be empty when we reach here
        TXBUF = value;
    } else {
        while(!tx_buffer.shift(value)) {
            // Buffer is full
            // Wait enough cycles for the UART controller to consume a byte, in theory
            __delay_cycles(BUFF_FULL_WAIT_TIME);
        }
        ENABLE_TX_IR; // this will force the ISR below to trigger at least once
    }
}

INLINE uint8_t receive() {
    uint8_t result;
    DISABLE_RX_IR; // disable this first before we check
    while(!rx_buffer.unshift(result)) {
        ENABLE_RX_IR; // now enable
        GO_TO_SLEEP;
    }
    ENABLE_RX_IR; // enable again
    return result;
}

INLINE void flush_tx() {
    // There's a very small chance that between us checking this flag and going to sleep
    // that the TX buf is now ready. This is OK though, as we specifically enable the TX IR flag
    // so when we go to sleep, the TX IFG is enabled and the ISR is triggered anyway
    // just means we've wasted a few cycles going to sleep and waking up again
    while(!(IS_TX_BUF_READY)) {
        __disable_interrupt();
        flushing = 1;   // request that the TX ISR wakes us up when it's finished transmitting
        ENABLE_TX_IR;   // ensure that the TX ISR is called at least once
        GO_TO_SLEEP;    // go to sleep, with GIE bits enabled
        __enable_interrupt(); // we've been woken up, all bytes are written, enable interrupts again
        // it's possible that something else woke us up so keep running the while loop
    }
}


/**
 * This is called when the TX buffer is ready to accept a new byte
 * So, if there are bytes in the TX buffer, then send those
 * Otherwise, there's nothing to do, disable the TX ISR
 */
INLINE void OnTxISR() {
    uint8_t result;
    // Don't unshift straight into volatile register, does some funky stuff
    if(tx_buffer.unshift(result)) {
        // New byte to transmit
        TXBUF = result;
    }
    if(tx_buffer.empty()){
        // All done, stop interrupts
        DISABLE_TX_IR;
        if(flushing) {
            flushing = 0;
            globals::REQUEST_CPU_AWAKE = 1;
        }
    }
}

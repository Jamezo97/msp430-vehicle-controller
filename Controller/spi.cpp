/*
 * spi.cpp
 *
 *  Created on: 6 Jun 2021
 *      Author: James Wyatt
 */

#include <msp430.h>
#include "spi.hpp"
#include "util.hpp"
#include "lib/shift_register.hpp"
#include "config.hpp"
#include "globals.hpp"

// See Table 36-4 in slau208q_msp_users_guide.pdf for these values
#define UCBBR (0) //8MHz on both devices, should we divide by 2??
#define UCBBITS (BIT5 | BIT6 | BIT7) // PIN 1.5=CLK, 1.6=SOMI, 1.7=SIMO

// Some macros to make life easier
#define IS_TX_BUF_READY (UC0IFG & UCB0TXIFG)
#define ENABLE_TX_IR (IE2 |= UCB0TXIE)
#define DISABLE_TX_IR (IE2 &= ~UCB0TXIE)
#define ENABLE_RX_IR (IE2 |= UCB0RXIE)
#define DISABLE_RX_IR (IE2 &= ~UCB0RXIE)
#define BUFF_FULL_WAIT_TIME (SMCLOCK_RATE / (SPI_CLOCK_RATE / 12))
#define TX_BUFF_SIZE (5)
#define RX_BUFF_SIZE (5)

#define BIT_CAN (BIT3)
#define BIT_POT (BIT4)

#define TXBUF UCB0TXBUF
#define RXBUF UCB0RXBUF

namespace spi {

void Init() {
    // Setup the chip selects

    // Pin 1.3 is the chip select, active low (so high means they're disabled)
    P1OUT |= (BIT_CAN | BIT_POT); // Drive them high
    P1DIR |= (BIT_CAN | BIT_POT); // make then outputs


    // Setup the pins for special mode
    P1SEL |= UCBBITS;
    P1SEL2 |= UCBBITS;

    UCB0CTL1 = UCSWRST;         // Put state-machine in reset mode
    UCB0CTL1 |= UCSSEL_2;       // SMCLK
    UCB0CTL0 = UCCKPH + UCMSB + UCMST + UCSYNC; // (Sync Mode Clock Phase), (MSB First), (Master), (SPI Mode)
    UCB0BR0 = UCBBR & 0xFF;     // Bit-Rate control registers
    UCB0BR1 = (UCBBR >> 8) & 0xFF;
    UCA0CTL1 &= ~UCSWRST;       // Clear reset flag, start state-machine

    ENABLE_RX_IR;
}

void chip_select_can(uint8_t enabled) {
    if(enabled) {
        P1OUT |= BIT_CAN;
    } else {
        P1OUT &= ~(BIT_CAN);
    }
}

void chip_select_pot(uint8_t enabled) {
    if(enabled) {
        P1OUT |= BIT_POT;
    } else {
        P1OUT &= ~(BIT_POT);
    }
}

#include "ucs.hpp"


void OnRxISR() {
    // Chuck it into our byte shift register
    uint8_t result = RXBUF; // reading this register will reset the RX ISR flag
    rx_buffer.shift(result);
    P1OUT ^= BIT0; // debug, toggle led
    globals::REQUEST_CPU_AWAKE = 1; // request the the ISR wakes up the device
}

}




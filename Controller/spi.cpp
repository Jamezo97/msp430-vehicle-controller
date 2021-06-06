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
#include "uart.hpp"

// See Table 36-4 in slau208q_msp_users_guide.pdf for these values
#define CLOCK_DIV   (SMCLOCK_RATE / SPI_CLOCK_RATE) // 8MHz on both devices, clock must be < half of this
#define UCBBITS     (BIT5 | BIT6 | BIT7) // PIN 1.5=CLK, 1.6=SOMI, 1.7=MOSI
// No modulation for UCB0
#define UCxxTXIFG   UCB0TXIFG
#define UCxxRXIFG   UCB0RXIFG
#define UCxxTXIE    UCB0TXIE
#define UCxxRXIE    UCB0RXIE
#define UCxxTXBUF   UCB0TXBUF
#define UCxxRXBUF   UCB0RXBUF
#define WAKEUP_xx_TX    WAKEUP_SPI_TX
#define WAKEUP_xx_RX    WAKEUP_SPI_RX
#define TX_BUFF_SIZE (4)
#define RX_BUFF_SIZE (4)

#define BIT_CAN (BIT3)
#define BIT_POT (BIT4)

namespace spi {

#include "comm.impl.hpp"

void Init() {
    // Setup the chip selects

    // Pin 1.3 is the chip select, active low (so high means they're disabled)
    P1OUT &= ~(BIT_CAN | BIT_POT); // Drive them low
    P1DIR |= (BIT_CAN | BIT_POT); // make then outputs

    // Setup the pins for special mode
    P1SEL |= UCBBITS;
    P1SEL2 |= UCBBITS;

    UCB0CTL1 = UCSWRST;         // Put state-machine in reset mode
    UCB0CTL1 |= UCSSEL_2;       // SMCLK
    UCB0CTL0 = UCMSB + UCMST + UCSYNC; // (MSB First), (Master), (SPI Mode)

    UCB0CTL0 |= UCCKPH;
//    UCB0CTL0 |= UCCKPL;

    UCB0BR0 = CLOCK_DIV & 0xFF;     // Bit-Rate control registers
    UCB0BR1 = (CLOCK_DIV >> 8) & 0xFF;
    UCB0CTL1 &= ~UCSWRST;       // Clear reset flag, start state-machine
    // From the MCP2515 documentation

    // Commands and data are sent to the device via the [Peripheral In]
    // pin, with data being clocked in on the rising edge of
    // [CLK]. Data is driven out by the MCP2515 (on the [Peripheral Out]
    // line) on the falling edge of [CLK]. The CS pin must be
    // held low while any operation is performed.

    ENABLE_RX_IR;
}


void chip_select_can(uint8_t selected) {
    if(selected) {
        P1OUT |= BIT_CAN;
    } else {
        P1OUT &= ~(BIT_CAN);
    }
}

void chip_select_pot(uint8_t selected) {
    if(selected) {
        P1OUT &= ~(BIT_POT);
    } else {
        P1OUT |= BIT_POT;
    }
}

void wait_response(uint8_t count) {
    __disable_interrupt();
    while(rx_buffer.size() < count) {
        __wakeup_on(WAKEUP_xx_RX);
        GO_TO_SLEEP;
    }
    __enable_interrupt();
}


void print_rx() {
    uint8_t result;
    uart::write("SPI RX = 0x");
    while(spi::countrx()) {
        spi::read(result);
        uart::writeHex(result);
    }
    uart::newline();
}

}




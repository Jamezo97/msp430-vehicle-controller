/*
 * uart.cpp
 *
 *  Created on: 5 Jun 2021
 *      Author: James Wyatt
 */


#include <msp430.h>
#include "uart.hpp"
#include "util.hpp"
#include "lib/shift_register.hpp"
#include "config.hpp"
#include "globals.hpp"

// See Table 36-4 in slau208q_msp_users_guide.pdf for these values
#define UCABR (69) //8MHz 115200
#define UCABITS (BIT1 | BIT2) // PIN 1.1=RX and 1.2=TX
#define UCBR (UCBRF_0 | UCBRS_4) // Modulation, FirstStage=0, SecondStage = 4. See Table 36-4 in slau208q_msp_users_guide.pdf

// Some macros to make life easier
#define IS_TX_BUF_READY (UC0IFG & UCA0TXIFG)
#define ENABLE_TX_IR (IE2 |= UCA0TXIE)
#define DISABLE_TX_IR (IE2 &= ~UCA0TXIE)
#define ENABLE_RX_IR (IE2 |= UCA0RXIE)
#define DISABLE_RX_IR (IE2 &= ~UCA0RXIE)
#define BUFF_FULL_WAIT_TIME (SMCLOCK_RATE / (UART_BAUD_RATE / 10))
#define TX_BUFF_SIZE (5)
#define RX_BUFF_SIZE (5)

#define TXBUF UCA0TXBUF
#define RXBUF UCA0RXBUF

namespace uart {

void Init() {
    constexpr bool EnableLoopback = false;

    // At 8000 MHz clock frequency, running UART at 115200
    // We get maximum TX/RX errors (%) of: -0.6 to 0.8,  -1.8 to 1.1  (respectively)
    P1SEL |= UCABITS;
    P1SEL2 |= UCABITS;
    UCA0CTL1 = UCSWRST;         // Put state-machine in reset mode
    UCA0CTL1 |= UCSSEL_2;       // SMCLK
    UCA0BR0 = UCABR & 0xFF;
    UCA0BR1 = (UCABR >> 8) & 0xFF;
    UCA0MCTL = UCBR;
    if(EnableLoopback) UCA0STAT |=  UCLISTEN;
    UCA0CTL1 &= ~UCSWRST;       // Clear reset flag, start statemachine

    ENABLE_RX_IR;
}

#include <ucs.hpp>

void OnRxISR() {
    // Chuck it into our byte shift register
    uint8_t result = RXBUF; // reading this register will reset the RX ISR flag
    rx_buffer.shift(result);
    P1OUT ^= BIT0; // debug, toggle led
    globals::REQUEST_CPU_AWAKE = 1; // request the the ISR wakes up the device
}

}




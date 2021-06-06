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
#define CLOCK_DIV   (69) //UCA0BRx - 8MHz 115200
#define UCABITS     (BIT1 | BIT2) // PIN 1.1=RX and 1.2=TX
#define UCBR        (UCBRF_0 | UCBRS_4) // Modulation, FirstStage=0, SecondStage = 4. See Table 36-4 in slau208q_msp_users_guide.pdf
#define UCxxTXIFG   UCA0TXIFG
#define UCxxRXIFG   UCA0RXIFG
#define UCxxTXIE    UCA0TXIE
#define UCxxRXIE    UCA0RXIE
#define UCxxTXBUF   UCA0TXBUF
#define UCxxRXBUF   UCA0RXBUF
#define WAKEUP_xx_TX    WAKEUP_UART_TX
#define WAKEUP_xx_RX    WAKEUP_UART_RX
#define TX_BUFF_SIZE (4)
#define RX_BUFF_SIZE (4)

namespace uart {

#include <comm.impl.hpp>

void Init() {
    constexpr bool EnableLoopback = false;

    // At 8000 MHz clock frequency, running UART at 115200
    // We get maximum TX/RX errors (%) of: -0.6 to 0.8,  -1.8 to 1.1  (respectively)
    P1SEL |= UCABITS;
    P1SEL2 |= UCABITS;
    UCA0CTL1 = UCSWRST;         // Put state-machine in reset mode
    UCA0CTL1 |= UCSSEL_2;       // SMCLK
    UCA0BR0 = CLOCK_DIV & 0xFF;
    UCA0BR1 = (CLOCK_DIV >> 8) & 0xFF;
    UCA0MCTL = UCBR;
    if(EnableLoopback) UCA0STAT |=  UCLISTEN;
    UCA0CTL1 &= ~UCSWRST;       // Clear reset flag, start statemachine

    ENABLE_RX_IR;
}

}




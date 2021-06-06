#include <msp430.h>

#include "uart.hpp"
#include "spi.hpp"
#include "can.hpp"
#include "util.hpp"
#include "config.hpp"
#include "globals.hpp"

uint8_t globals::REQUEST_CPU_AWAKE = 0;

// for debugging we can trigger things with UART RX/TX

void commandLoop() {

    uint8_t command = 0;
    for(;;) {
        GO_TO_SLEEP;
        command = uart::receive();
        if(command == 'r') {
            can::reset();
            spi::flush_tx();
            uart::write("Reset CAN\r\n");
            uart::flush_tx();
        } else if(command == 'w') {
            ENABLE_WATCHDOG;
            uart::write("Set WatchdogA\r\n");
            uart::write("Set WatchdogB\r\n");
            uart::write("Set WatchdogC\r\n");
            uart::write("Set WatchdogD\r\n");
            uart::write("Set WatchdogE\r\n");
            uart::write("Set WatchdogF\r\n");
            uart::write("Set WatchdogG\r\n");
            uart::write("Set WatchdogH\r\n");
            uart::flush_tx();
        } else {
            uart::write("Unknown command: ");
            uart::writeUint8(command);
            uart::newline();
            uart::flush_tx();
        }
    }
}

int main(void)
{
    DISABLE_WATCHDOG;

    if (MCLK_CALB == 0xFF) {
        while(1);
    }

    // Calibrate clocks
    DCOCTL = 0; // Lowest DCO
    BCSCTL1 = MCLK_CALB;
    DCOCTL = SMCLK_CALB;

    // Clear all pin data
    P1OUT = 0x00;
    P1DIR = 0xFF;
    P2OUT = 0x00;
    P2DIR = 0xFF;
    P3OUT = 0x00;
    P3DIR = 0xFF;


    // For debug, setup some pins
    P1DIR |= BIT0;
    P1OUT &= ~(BIT0);

    __enable_interrupt();

    uart::Init();
    spi::Init();

    uart::write("Hi\r\n");
    commandLoop();
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void TransmitInterrupt(void)
{
    // Because both UCA0 and UCB0 both use the same ISR we need to check
    // not only the flag, but also if the interrupt was enabled?
    if((UC0IFG & UCA0TXIFG) && (UC0IE & UCA0TXIE)) {
        uart::OnTxISR();
    }
    if((UC0IFG & UCB0TXIFG) && (UC0IE & UCB0TXIE)) {
        spi::OnTxISR();
    }

    if(globals::REQUEST_CPU_AWAKE) {
        globals::REQUEST_CPU_AWAKE = 0;
        GO_TO_AWAKE;
    }
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void ReceiveInterrupt(void)
{
    if(UC0IFG & UCA0RXIFG) {
        uart::OnRxISR();
    }
    if((UC0IFG & UCB0RXIFG) && (UC0IE & UCB0RXIE)) {
        spi::OnRxISR();
        UC0IFG &= ~UCB0RXIFG; // Clear RX flag
    }

    if(globals::REQUEST_CPU_AWAKE) {
        globals::REQUEST_CPU_AWAKE = 0;
        GO_TO_AWAKE;
    }
}












//
//// Base program size, with nothing in it, 1.7kB
//// MSP430:  Flash/FRAM usage is 1738 bytes. RAM usage is 350 bytes.
//#include "config.hpp"
//#include <stdint.h>
////#include "pins.hpp"
////#include "comm.hpp"
////#include "timer.hpp"
////#include "lib/event_loop.hpp"
//
//void setBaudRateScaler(uint16_t div) {
//    UCA0BR0 = div & 0xFF;
//    UCA0BR1 = (div >> 8) & 0xFF;
//}
//
//
//void print(char *text)
//{
//    unsigned int i = 0;
//    while(text[i] != '\0')
//    {
//        while (!(IFG2&UCA0TXIFG));      // Check if TX is ongoing
//        UCA0TXBUF = text[i];            // TX -> Received Char + 1
//        i++;
//    }
//}
//
//void printNumber(unsigned int num)
//{
//    char buf[6];
//    char *str = &buf[5];
//
//    *str = '\0';
//
//    do
//    {
//        unsigned long m = num;
//        num /= 10;
//        char c = (m - 10 * num) + '0';
//        *--str = c;
//    } while(num);
//
//    print(str);
//}
//
///**
// * main.c
// */
//int main(void)
//{
//	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
//
//	if (MCLK_CALB == 0xFF) { // Check if calibration constant erased
//        while(1);            // do not load program
//    }
//
//	DCOCTL = 0; // Lowest DCO
//	// Config clocks
//    BCSCTL1 = MCLK_CALB;
//    DCOCTL = SMCLK_CALB;
//    // Pin3 isn't initialized, so configure to output pins as per MSP documentation to be outputs
//    P3OUT = 0x00;
//    P3DIR = 0xFF;
//    // Enable UART special registers
//    P1SEL = BIT1 + BIT2;  // Select UART RX/TX function on P1.1,P1.2
//    P1SEL2 = BIT1 + BIT2;
//
//    UCA0CTL1 |= UCSSEL_2;               // UART Clock -> SMCLK
//    setBaudRateScaler(MCLOCK_RATE / UART_BAUD_RATE); // Baud Rate Setting for 1MHz 9600
//    UCA0MCTL = UCBRS_1;                 // Modulation Setting for 1MHz 9600
//    UCA0CTL1 &= ~UCSWRST;
//
////    PIN_CAN_INTERRUPT.configureInterrupt(Edge::Falling, 1, 0, Resistor::PullUp);
//
////    UART.write('a');
//
//	// Forever...
//    unsigned int count;
//	for(;;) {
//	    // We've been waken up, service the event loop
////	    EVENT_LOOP.service();
////	    __delay_cycles(8000);
//
//        printNumber(count);
//        print("\r\n");
//        count++;
//        __delay_cycles(100000);
//
//	    // Nothing more to do. Back to sleep
////	    _enable_interrupt();
////	    __bis_SR_register(LPM0_bits + GIE);
////        _disable_interrupt();
////	    __no_operation(); // for debugging
//	}
//}
//
////#pragma vector=PORT2_VECTOR
////__interrupt void Port_2(void)
////{
////    // this is hopefully all inlined
////    if(PIN_CAN_INTERRUPT.clearInterruptFlag()) {
////        __bic_SR_register_on_exit(LPM0_bits); // Wake up the CPU
////    }
////}



//******************************************************************************
//   MSP430G2xx3 Demo - USCI_A0, 9600 UART Echo ISR, DCO SMCLK
//
//   Description: Echo a received character, RX ISR used. Normal mode is LPM0.
//   USCI_A0 RX interrupt triggers TX Echo.
//   Baud rate divider with 1MHz = 1MHz/9600 = ~104.2
//   ACLK = n/a, MCLK = SMCLK = CALxxx_1MHZ = 1MHz
//
//                MSP430G2xx3
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |     P1.2/UCA0TXD|------------>
//            |                 | 9600 - 8N1
//            |     P1.1/UCA0RXD|<------------
//
//   D. Dang
//   Texas Instruments Inc.
//   February 2011
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************

//void main(void)
//{
//    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
//    BCSCTL1 = MCLK_CALB;                    // Set DCO
//    DCOCTL = CALDCO_1MHZ;
//
//
//    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
//
//    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
//}

//  Echo back RXed character, confirm TX buffer is ready first
//#pragma vector=USCIAB0RX_VECTOR
//__interrupt void USCI0RX_ISR(void)
//{
//    while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
//    UCA0TXBUF = UCA0RXBUF;                    // TX -> RXed character
//}


//void main(void)
//{
//  WDTCTL = WDTPW + WDTHOLD; // Stop the Watch dog
//
//  //------------------- Configure the Clocks -------------------//
//
//  if (CALBC1_1MHZ==0xFF)   // If calibration constant erased
//     {
//        while(1);          // do not load, trap CPU!!
//     }
//
//   DCOCTL  = 0;             // Select lowest DCOx and MODx settings
//   BCSCTL1 = CALBC1_1MHZ;   // Set range
//   DCOCTL  = CALDCO_1MHZ;   // Set DCO step + modulation
//
//  //---------------- Configuring the LED's ----------------------//
//
//   P1DIR  |=  BIT0 + BIT6;  // P1.0 and P1.6 output
//   P1OUT  &= ~BIT0 + BIT6;  // P1.0 and P1.6 = 0
//
//  //--------- Setting the UART function for P1.1 & P1.2 --------//
//
//   P1SEL  |=  BIT1 + BIT2;  // P1.1 UCA0RXD input
//   P1SEL2 |=  BIT1 + BIT2;  // P1.2 UCA0TXD output
//
//  //------------ Configuring the UART(USCI_A0) ----------------//
//
//   UCA0CTL1 |=  UCSSEL_2 + UCSWRST;  // USCI Clock = SMCLK,USCI_A0 disabled
//   UCA0BR0   =  104;                 // 104 From datasheet table-
//   UCA0BR1   =  0;                   // -selects baudrate =9600,clk = SMCLK
//   UCA0MCTL  =  UCBRS_1;             // Modulation value = 1 from datasheet
////   UCA0STAT |=  UCLISTEN;            // loop back mode enabled
//   UCA0CTL1 &= ~UCSWRST;             // Clear UCSWRST to enable USCI_A0
//
//  //---------------- Enabling the interrupts ------------------//
//
//   IE2 |= UCA0TXIE;                  // Enable the Transmit interrupt
//   IE2 |= UCA0RXIE;                  // Enable the Receive  interrupt
//   _BIS_SR(GIE);                     // Enable the global interrupt
//
//   UCA0TXBUF = 'A';                  // Transmit a byte
//
//   _BIS_SR(LPM0_bits + GIE);         // Going to LPM0
//}
//
//  //-----------------------------------------------------------------------//
//  //                Transmit and Receive interrupts                        //
//  //-----------------------------------------------------------------------//
//
//  #pragma vector = USCIAB0TX_VECTOR
//  __interrupt void TransmitInterrupt(void)
//  {
//    P1OUT  ^= BIT0;//light up P1.0 Led on Tx
//  }
//
//  #pragma vector = USCIAB0RX_VECTOR
//  __interrupt void ReceiveInterrupt(void)
//  {
//    P1OUT  ^= BIT6;     // light up P1.6 LED on RX
//    IFG2 &= ~UCA0RXIFG; // Clear RX flag
//  }

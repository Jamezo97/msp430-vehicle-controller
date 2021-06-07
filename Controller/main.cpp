#include <msp430.h>

#include "uart.hpp"
#include "spi.hpp"
#include "can.hpp"
#include "util.hpp"
#include "config.hpp"
#include "globals.hpp"

uint8_t globals::WAKEUP_FLAGS = 0;

// for debugging we can trigger things with UART RX/TX

void commandLoop() {

    uint8_t command = 0;
    for(;;) {
        command = uart::receive();
        if(command == 'r') {
            can::reset();
            spi::flush_tx();
            uart::write("Reset CAN\r\n");
            uart::flush_tx();
        } else if(command == 'W') {
            ENABLE_WATCHDOG;
            uart::write("Set WatchdogA\r\n");
            uart::write("Set WatchdogB\r\n");
            uart::write("Set WatchdogC\r\n");
            uart::write("Set WatchdogD\r\n"); // Dies about here, that's good
            uart::write("Set WatchdogE\r\n");
            uart::write("Set WatchdogF\r\n");
            uart::write("Set WatchdogG\r\n");
            uart::write("Set WatchdogH\r\n");
            uart::flush_tx();
        } else if(command == 'l') {
            uint8_t count = 0;
            do {
                count--;
                uart::writeHex(count);
                uart::write("Hello World This Is A Long String\r\n", 35);
            } while(count);
        } else if(command == 't') {
            uart::write("Test: ");
            uint8_t result = can::test(0xab);
            uart::writeHex(result);
            uart::newline();
        } else if(command == 'p') {
            spi::print_rx();
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

    // by default we enable interrupts while awake
    __enable_interrupt();

    uart::Init();
    spi::Init();

    uart::delay_cycles(500);

    uart::write("Hi\r\n");
    commandLoop();
    return;
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void TransmitInterrupt(void)
{
    uint8_t wakeup_mask = 0;
    // Because both UCA0 and UCB0 both use the same ISR we need to check
    // not only the flag, but also if the interrupt was enabled?
    if((UC0IFG & UCA0TXIFG) && (UC0IE & UCA0TXIE)) {
        uart::OnTxISR();
        wakeup_mask |= WAKEUP_UART_TX;
    }
    if((UC0IFG & UCB0TXIFG) && (UC0IE & UCB0TXIE)) {
        spi::OnTxISR();
        wakeup_mask |= WAKEUP_SPI_TX;
    }

    if(globals::WAKEUP_FLAGS & wakeup_mask) {
        globals::WAKEUP_FLAGS &= ~wakeup_mask;
        GO_TO_AWAKE;
    }
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void ReceiveInterrupt(void)
{
    uint8_t wakeup_mask = 0;
    if((UC0IFG & UCA0RXIFG) && (UC0IE & UCA0RXIE)) {
        uart::OnRxISR();
        wakeup_mask |= WAKEUP_UART_RX;
    }
    if((UC0IFG & UCB0RXIFG) && (UC0IE & UCB0RXIE)) {
        spi::OnRxISR();
        wakeup_mask |= WAKEUP_SPI_RX;
    }

    if(globals::WAKEUP_FLAGS & wakeup_mask) {
        globals::WAKEUP_FLAGS &= ~wakeup_mask;
        GO_TO_AWAKE;
    }
}


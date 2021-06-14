#include <msp430.h>

#include "uart.hpp"
#include "spi.hpp"
#include "can.hpp"
#include "util.hpp"
#include "config.hpp"
#include "globals.hpp"

volatile uint8_t globals::WAKEUP_FLAGS = 0;
volatile uint8_t globals::WAKEUP_COMMAND = 0;

union TestUnion {
    uint8_t value;
    struct {
        uint8_t bit0 : 1;
        uint8_t bit1 : 1;
        uint8_t bit2 : 1;
        uint8_t bit3 : 1;
        uint8_t bit4 : 1;
        uint8_t bit5 : 1;
        uint8_t bit6 : 1;
        uint8_t bit7 : 1;
    } bits;
};

// for debugging we can trigger things with UART RX/TX

void print_can(CANMSG& msg) {
    uart::write("=== CAN MSG ===\r\n");
//    uint8_t tmp[CANMSG::SIZE];
//    msg.marshal(tmp);
//    uart::writeHexArray(tmp, CANMSG::SIZE);
//    uart::newline();

    uart::writeKeyValue("CAN ID", msg.id.SID);
//    uart::writeKeyValue("SRR", msg.id.SRR);
//    uart::writeKeyValue("IDE", msg.id.IDE);
//    uart::writeKeyValue("EID", msg.id.EID);

//    uart::writeKeyValue("RTR", msg.dlc.RTR);
    uart::writeKeyValue("DLC", msg.dlc.DLC);

    uart::write("Data: ");
    uart::write((char*)msg.data, msg.dlc.DLC);
    uart::newline();
}

void commandReadUart() {
    uint8_t command;
    while(uart::read(command)) {
        if(command == 'r') {
            can::reset_mcp2515();
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
        } else if(command == 'p') {
            spi::print_rx();
        } else if(command >= '1' && command <= '8') {
            uint8_t DLC = command - '1' + 1;
            uart::write("\r\n\r\n========== DLC: ");
            uart::writeHex(DLC);
            uart::newline();

            CANMSG msg;
            msg.id = 0x3FF;
//            msg.id.EID = 0b100000000000000001;
//            msg.id.IDE = 1;
            msg.dlc = DLC;
            msg.data[0] = 'A'+DLC-1;
            msg.data[1] = 'B'+DLC-1;
            msg.data[2] = 'C'+DLC-1;
            msg.data[3] = 'D'+DLC-1;
            msg.data[4] = 'E'+DLC-1;
            msg.data[5] = 'F'+DLC-1;
            msg.data[6] = 'G'+DLC-1;
            msg.data[7] = 'H'+DLC-1;
            // fill buffer
            can::tx_buffer0(msg);
            print_can(msg);

            // read the data back out
//            can::read(0b00110001, (uint8_t*)(&msg), CANMSG::SIZE);
//            print_can(msg);



            can::requestToSend(BIT0);
        } else if(command == 'c') {
            uint8_t tmp[4];
            CANIdentifier id;
            id.EID = 0b100000000000000001;
            id.marshal(tmp);
            CANIdentifier result;
            result.unmarshal(tmp);

            uart::write("Bytes: ");
            uart::writeHexArray(tmp, 4);
            uart::newline();
            uart::writeKeyValue("EID Before", id.EID);
            uart::writeKeyValue("EID  After", result.EID);


//            CANMSG msg;
//            uint8_t offset_0 = (unsigned long)(&msg.id) - (unsigned long)(&msg);
//            uint8_t offset_1 = (unsigned long)(&msg.dlc) - (unsigned long)(&msg);
//            uint8_t offset_2 = (unsigned long)(&msg.data[0]) - (unsigned long)(&msg);
//
//            uart::writeKeyValue("ID", offset_0);
//            uart::writeKeyValue("DLC", offset_1);
//            uart::writeKeyValue("DATA", offset_2);


        } else {
            uart::write("Unknown command: ");
            uart::writeUint8(command);
            uart::newline();
            uart::flush_tx();
        }
    }
}

void commandReadCAN() {
    CANMSG msg;
    uint8_t resultcode = can::rx_pending(msg);

    if(resultcode) {
        print_can(msg);
    } else {
        uart::write("Failed to read?\r\n");
    }
}

void commandLoop() {
    __wakeup_on(WAKEUP_UART_RX);
    for(;;) {
        GO_TO_SLEEP;
        while(globals::WAKEUP_COMMAND) {
            if(globals::WAKEUP_COMMAND & COMMAND_READ_UART) {
                // Commanded to read a byte from the UART RX buffer
                globals::WAKEUP_COMMAND &= ~COMMAND_READ_UART;
                __wakeup_on(WAKEUP_UART_RX);
                commandReadUart();
            }
            if(globals::WAKEUP_COMMAND & COMMAND_READ_CAN) {
                // Commanded to process some CAN data
                globals::WAKEUP_COMMAND &= ~COMMAND_READ_CAN;
                commandReadCAN();
            }
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
    can::Init();

    uart::delay_cycles(500);

    uart::write("Hi\r\n");
    commandLoop();
    return 0;
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
        globals::WAKEUP_COMMAND |= COMMAND_READ_UART; // process UART RX buffer
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


#pragma vector = PORT2_VECTOR
__interrupt void Port2Interrupt(void)
{
    // CAN port interrupt
    if(P2IFG & BIT0) {
        globals::WAKEUP_COMMAND |= COMMAND_READ_CAN;
        P2IFG &= ~BIT0; // we must manually clear the bit to prevent this interrupt being triggered again
        GO_TO_AWAKE;
    }
}


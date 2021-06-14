/*
 * can.cpp
 *
 *  Created on: 6 Jun 2021
 *      Author: James Wyatt
 */
#include "util.hpp"
#include "can.hpp"
#include "spi.hpp"
#include "can_cmd.hpp"
#include "globals.hpp"


namespace can {

void Init() {
    // There are only 2 RX buffers on the MCP2515, we need to use these carefully
    // To maximize the LPM efficiency, one buffer can be used to receive the wake-up CAN message
    // up receiving such a message, it generates an interrupt and will wake the MCU to read and process
    // Buffer 0 is the higher priority buffer, we can put the wakeup messages in there
    // Buffer 1 has more filters, so we can specifically filter just the messages we require

    // Or not, turns out in listen mode the masks and filters aren't used
    // I assume in normal mode if we accept a message then we'll ACK it, which I don't
    // want to do, at least not yet. For now we can read all messages and list them?


    // Configure the CAN interrupt pin
    P2DIR &= ~BIT0; // it's an input
    P2OUT |= BIT0; // pullup resistor
    P2REN |= BIT0; // enable pullup resistor
    P2IES |= BIT0; // CAN interrupt is active LOW, so on falling edge trigger interrupt
    P2IFG &= ~BIT0; // clear the flag
    P2IE |= BIT0; // enable interrupt for PIN2.0

    // Configure the CAN controller
    // We sent a reset command, so we're in configuration state

    reset_mcp2515();
}

void reset_mcp2515() {
    spi::CanTransmission();
    spi::transmit(CMD_RESET);

    // For now, lets put it into loopback mode
    setCanControl(CAN_OPMODE_LOOPBACK);
    setIE(CAN_INT_RX0);
}


uint8_t is_interrupt_pending() {
    // In the event that multiple interrupts occur, the INT pin will remain low until all interrupts have been reset by the MCU.
    if(P2IN & BIT0) {
        return 0; // if it's high, then interrupt is not pending
    } else {
        return 1;
    }
}

void rx_buffer(CANMSG& msg, uint8_t CMD_RXBxSIDH) {
    spi::CanTransmission();
    spi::transmit(CMD_RXBxSIDH);
    for(uint8_t fill = CANMSG::SIZE; fill > 0; --fill) {
        spi::transmit(0);
    }
    spi::receive(); // skip the byte that comes from the command
    uint8_t rxBuffer[CANMSG::SIZE];
    spi::receive(rxBuffer, CANMSG::SIZE);
    msg.unmarshal(rxBuffer);
}

// Read RX buffer 0
void rx_buffer0(CANMSG& msg) {
    rx_buffer(msg, CMD_READ_RX | RX_RXB0SIDH);
}

// Read RX buffer 1
void rx_buffer1(CANMSG& msg) {
    rx_buffer(msg, CMD_READ_RX | RX_RXB1SIDH);
}

// return true if more are pending
uint8_t rx_pending(CANMSG& msg) {
    CANInterruptStatus ifg;
    readInterruptStatus(ifg);

    if(ifg.RX0IFG) {
        rx_buffer0(msg);
//        bit_modify(REG_CANINTF, CAN_INT_RX0, ~CAN_INT_RX0); // clear the flag
        if(ifg.RX1IFG) {
            return 1 | CAN_FLG_MORE_PENDING;
        } else {
            return 1;
        }
    } else if(ifg.RX1IFG) {
        rx_buffer1(msg);
//        bit_modify(REG_CANINTF, CAN_INT_RX1, ~CAN_INT_RX1); // clear the flag
        return 2; // read the second (index 1) buffer
    } else {
        return 0; // didn't read
    }
}

void tx_buffer(CANMSG& msg, uint8_t CMD_TXBxSIDH) {
    uint8_t buffer[CANMSG::SIZE];
    msg.marshal(buffer);
    spi::CanTransmission();
    spi::transmit(CMD_TXBxSIDH);
    spi::transmit(buffer, CANMSG::SIZE);
}

void tx_buffer0(CANMSG& data) {
    tx_buffer(data, CMD_LOAD_TX | TX_TXB0SIDH);
}
void tx_buffer1(CANMSG& data) {
    tx_buffer(data, CMD_LOAD_TX | TX_TXB1SIDH);
}
void tx_buffer2(CANMSG& data) {
    tx_buffer(data, CMD_LOAD_TX | TX_TXB2SIDH);
}

// BIT0, BIT1, BIT2 for TX buffer 0,1,2 respectively
void requestToSend(uint8_t buffers) {
    spi::CanTransmission();
    spi::transmit(CMD_RTS | (buffers & 0b111)); // AND the first 3 bits so we don't break the command
}

// At address, set bits specified in `mask` to value specified in `data`
void bit_modify(uint8_t address, uint8_t mask, uint8_t data) {
    spi::CanTransmission();
    spi::transmit(CMD_BIT_MODIFY);
    spi::transmit(mask);
    spi::transmit(data);
}

void setCanControl(uint8_t value) {
    write(REG_CANCTRL, &value, 1);
}

void setIE(uint8_t flags) {
    write(REG_CANINTE, &flags, 1);
}

uint8_t getCanStat() {
    uint8_t result;
    read(REG_CANSTAT, &result, 1);
    return result;
}

void readInterruptStatus(CANInterruptStatus& result) {
    spi::CanTransmission();
    spi::transmit(CMD_READ_STATUS);
    spi::transmit(0);
    spi::receive(); // skip the high impedence state
    spi::receive((uint8_t*)(&result), 1); // read into the result struct
}
void readRxStatus(CANReceiveStatus& result) {
    spi::CanTransmission();
    spi::transmit(CMD_RX_STATUS);
    spi::transmit(0);
    spi::receive(); // skip the high impedence state
    spi::receive((uint8_t*)(&result), 1); // read into the result struct
}

// Read an arbitrary address
void read(uint8_t address, uint8_t* result, uint8_t count) {
    spi::CanTransmission();
    spi::transmit(CMD_READ);
    spi::transmit(address);
    for(uint8_t fill = count; fill > 0; --fill) {
        spi::transmit(0);
    }
    // Wait for the first 2 don't-care bytes
    spi::receive();
    spi::receive();
    // receive data as it arrives
    spi::receive(result, count);
}

void write(uint8_t address, uint8_t* data, uint8_t count) {
    spi::CanTransmission();
    spi::transmit(CMD_WRITE);
    spi::transmit(address);
    spi::transmit(data, count);
}


uint8_t test(uint8_t value) {
    write(REG_TXB0D0, &value, 1);
    uint8_t result;
    read(REG_TXB0D0, &result, 1);
    return result;
}

}




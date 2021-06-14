/*
 * can_cmd.hpp
 *
 *  Created on: 6 Jun 2021
 *      Author: James Wyatt
 */

#ifndef CAN_CMD_HPP_
#define CAN_CMD_HPP_

#define CMD_RESET       (0b11000000) // Reset the controller
#define CMD_READ        (0b00000011) // Read consecutive bytes, starting at the address specified by the second sent byte
#define CMD_READ_RX     (0b10010000) // OR this with RX_RXBx
#define CMD_WRITE       (0b00000010) // <Write> <Address> <Value> [Value m+1] [Value m+2]...
#define CMD_LOAD_TX     (0b01000000) // OR this with TX_TXBx
#define CMD_RTS         (0b10000000) // fill last 3 bits with flags for 3 TX buffers
#define CMD_READ_STATUS (0b10100000) // Quick polling command that reads several status bits for transmit and receive functions.
#define CMD_RX_STATUS   (0b10110000) // Quick polling command that indicates filter match and message type (standard, extended and/or remote) of received message.
#define CMD_BIT_MODIFY  (0b00000101) // Allows the user to set or clear individual bits in a particular register

#define RX_RXB0SIDH (0b000) // address 0x61
#define RX_RXB0D0   (0b010) // address 0x66
#define RX_RXB1SIDH (0b100) // address 0x71
#define RX_RXB1D0   (0b110) // address 0x76

#define TX_TXB0SIDH (0b000) // address 0x31
#define TX_TXB0D0   (0b001) // address 0x36
#define TX_TXB1SIDH (0b010) // address 0x41
#define TX_TXB1D0   (0b011) // address 0x46
#define TX_TXB2SIDH (0b100) // address 0x51
#define TX_TXB2D0   (0b101) // address 0x56

#define CAN_OPMODE_NORMAL   (0b00000000)
#define CAN_OPMODE_SLEEP    (0b00100000)
#define CAN_OPMODE_LOOPBACK (0b01000000)
#define CAN_OPMODE_LISTEN   (0b01100000)
#define CAN_OPMODE_CONFIG   (0b10000000)

// Same bits for INTF and INTE registers
#define CAN_INT_RX0      (0x01)
#define CAN_INT_RX1      (0x02)
#define CAN_INT_TX0      (0x04)
#define CAN_INT_TX1      (0x08)
#define CAN_INT_TX2      (0x10)
#define CAN_INT_ERRIE    (0x20)
#define CAN_INT_WAKIE    (0x40)
#define CAN_INT_MERRA    (0x80)

#define REG_CANINTE (0b00101011) // interrupt enable register
#define REG_CANINTF (0b00101100) // interrupt flag register
#define REG_RXF0 (0b00000000) // Rx Filter 0, SIDH, SIDL, EID8, EID0
#define REG_RXF1 (0b00000100) // Rx Filter 1, SIDH, SIDL, EID8, EID0
#define REG_RXF2 (0b00001000) // Rx Filter 2, SIDH, SIDL, EID8, EID0
#define REG_RXF3 (0b00010000) // Rx Filter 3, SIDH, SIDL, EID8, EID0
#define REG_RXF4 (0b00010100) // Rx Filter 4, SIDH, SIDL, EID8, EID0
#define REG_RXF5 (0b00011000) // Rx Filter 5, SIDH, SIDL, EID8, EID0

#define REG_RXM0 (0b00100000) // Rx Mask 0, SIDH, SIDL, EID8, EID0
#define REG_RXM1 (0b00100100) // Rx Mask 1, SIDH, SIDL, EID8, EID0

#define REG_CANSTAT (0x0E)
#define REG_CANCTRL (0x0F)


#define REG_TXB0D0  (0b00110110) // Transmit buffer 0, data byte 0

#endif /* CAN_CMD_HPP_ */

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

#endif /* CAN_CMD_HPP_ */

/*
 * can.hpp
 *
 *  Created on: 6 Jun 2021
 *      Author: James Wyatt
 */

#ifndef CAN_HPP_
#define CAN_HPP_

#include "can_types.hpp"

#define CAN_FLG_MORE_PENDING (0x04)

namespace can {

void Init();
void reset_mcp2515();
uint8_t test(uint8_t value);

void read(uint8_t address, uint8_t* result, uint8_t count);
void write(uint8_t address, uint8_t* data, uint8_t count);

void rx_buffer0(CANMSG& result);
void rx_buffer1(CANMSG& result);
uint8_t rx_pending(CANMSG& result); // return true if more are pending
void tx_buffer0(CANMSG& data);
void tx_buffer1(CANMSG& data);
void tx_buffer2(CANMSG& data);
uint8_t is_interrupt_pending();
void requestToSend(uint8_t buffers);
void setIE(uint8_t flags);
// At address, set bits specified in `mask` to value specified in `data`
void bit_modify(uint8_t address, uint8_t mask, uint8_t data);

void setCanControl(uint8_t value);
uint8_t getCanStat();

void readInterruptStatus(CANInterruptStatus& result);
void readRxStatus(CANReceiveStatus& result);

}


#endif /* CAN_HPP_ */

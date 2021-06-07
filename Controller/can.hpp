/*
 * can.hpp
 *
 *  Created on: 6 Jun 2021
 *      Author: James Wyatt
 */

#ifndef CAN_HPP_
#define CAN_HPP_

//#include <stdint.h>

namespace can {

void reset();
uint8_t test(uint8_t value);

void read(uint8_t address, uint8_t* result, uint8_t count);
void read(uint8_t address, uint8_t count);
void write(uint8_t address, uint8_t* data, uint8_t count);

}

#endif /* CAN_HPP_ */

/*
 * can.hpp
 *
 *  Created on: 6 Jun 2021
 *      Author: James Wyatt
 */

#ifndef CAN_HPP_
#define CAN_HPP_

namespace can {

void reset();
void request_read();
void request_write(uint8_t value);

}

#endif /* CAN_HPP_ */

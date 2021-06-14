/*
 * globals.hpp
 *
 *  Created on: 5 Jun 2021
 *      Author: James Wyatt
 */

#ifndef GLOBALS_HPP_
#define GLOBALS_HPP_

#include <stdint.h>

#define WAKEUP_UART_TX  (0x01)
#define WAKEUP_UART_RX  (0x02)
#define WAKEUP_SPI_TX   (0x04)
#define WAKEUP_SPI_RX   (0x08)
#define WAKEUP_UART_TX_EMPTY    (0x10)
#define WAKEUP_SPI_TX_EMPTY     (0x20)

#define COMMAND_READ_UART (0x01)
#define COMMAND_READ_CAN (0x02)

namespace globals {

extern volatile uint8_t WAKEUP_FLAGS;
extern volatile uint8_t WAKEUP_COMMAND;

}

inline void __wakeup_on(uint8_t flag) {
    globals::WAKEUP_FLAGS |= flag;
}



#endif /* GLOBALS_HPP_ */

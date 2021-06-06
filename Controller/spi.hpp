/*
 * spi.hpp
 *
 *  Created on: 6 Jun 2021
 *      Author: James Wyatt
 */

#ifndef SPI_HPP_
#define SPI_HPP_

#include <stdint.h>

namespace spi {

void Init();

void chip_select_can(uint8_t enabled);
void chip_select_pot(uint8_t enabled);

void transmit(uint8_t value);
uint8_t receive();
void flush_tx();

void OnTxISR();
void OnRxISR();

}



#endif /* SPI_HPP_ */

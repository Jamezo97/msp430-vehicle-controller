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


void transmit(const uint8_t* data, unsigned int count);
void transmit(uint8_t value);
void receive(uint8_t* data, unsigned int count);
uint8_t receive();
void flush_tx();
uint8_t countrx();
uint8_t counttx();

void seek(uint8_t count);
bool read(uint8_t& result);
void wait(uint8_t countBytes);
void delay_cycles(unsigned int count);

void clear_rx();
void clear_tx();

void prepare();
void wait_response();
void print_rx();

void OnTxISR();
void OnRxISR();

void chip_select_can(uint8_t enabled);
void chip_select_pot(uint8_t enabled);


class CanTransmission {
public:
    CanTransmission();
    ~CanTransmission();
};


}



#endif /* SPI_HPP_ */

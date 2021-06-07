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

namespace can {

void Init() {
    reset();
}

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

void read(uint8_t address, uint8_t count) {
    spi::CanTransmission();
    spi::transmit(CMD_READ);
    spi::transmit(address);
    for(uint8_t fill = count; fill > 0; --fill) {
        spi::transmit(0);
    }
}

void write(uint8_t address, uint8_t* data, uint8_t count) {
    spi::CanTransmission();
    spi::transmit(CMD_WRITE);
    spi::transmit(address);
    spi::transmit(data, count);
}

void reset() {
    spi::CanTransmission();
    spi::transmit(CMD_RESET);
}

uint8_t test(uint8_t value) {
    write(REG_TXB0D0, &value, 1);
    uint8_t result;
    read(REG_TXB0D0, &result, 1);
    return result;
}

}




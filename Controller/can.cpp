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

class ChipSelectGuard {
public:
    ChipSelectGuard() {
        spi::chip_select_can(1);
        spi::delay_cycles(1);
    }
    ~ChipSelectGuard() {
        spi::flush_tx();
        spi::delay_cycles(1);
        spi::chip_select_can(0);
    }
};

void Init() {
    reset();
}

void reset() {
    ChipSelectGuard();
    spi::transmit(CMD_RESET);
}

void request_read() {
    ChipSelectGuard();
    spi::transmit(CMD_READ);
    spi::transmit(REG_TXB0D0);
    spi::transmit(0);
    spi::wait_response(3);
}

void request_write(uint8_t value) {
    ChipSelectGuard();
    spi::transmit(CMD_WRITE);
    spi::transmit(REG_TXB0D0);
    spi::transmit(value);
}


}




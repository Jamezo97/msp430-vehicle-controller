/*
 * can.cpp
 *
 *  Created on: 6 Jun 2021
 *      Author: James Wyatt
 */

#include "can.hpp"
#include "spi.hpp"
#include "can_cmd.hpp"

namespace can {

void Init() {
    reset();
}

void reset() {
    spi::chip_select_can(1);
    spi::transmit(CMD_RESET);
    spi::flush_tx();
    spi::chip_select_can(0);
}


}




/*
 * util.hpp
 *
 *  Created on: 5 Jun 2021
 *      Author: James Wyatt
 */

#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <stdint.h>
#include <msp430.h>

#define WDT_SRC_SMCLK   (0b00000000) // 0x00
#define WDT_SRC_ACLK    (0b00100000) // 0x20
#define WDT_RESET       (0b00001000) // 0x08
#define WDT_MSEL_TIMER  (0b00010000) // 0x10

#define GO_TO_SLEEP     (__bis_SR_register(LPM0_bits + GIE))
#define GO_TO_AWAKE     (__bic_SR_register_on_exit(LPM0_bits))
#define ENABLE_WATCHDOG     (WDTCTL = WDTPW + WDT_SRC_SMCLK + WDT_RESET) // WDTPW | WDTCNTCL)
#define DISABLE_WATCHDOG    (WDTCTL = WDTPW + WDTHOLD) // Password, Hold

inline void setU16From2U8(uint16_t value, volatile uint8_t& b0, volatile uint8_t& b1) {
    b0 = value & 0xFF;
    b1 = (value >> 8) & 0xFF;
}


#endif /* UTIL_HPP_ */

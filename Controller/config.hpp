/*
 * config.hpp
 *
 *  Created on: 30 May 2021
 *      Author: James Wyatt
 */

#ifndef CONFIG_HPP_
#define CONFIG_HPP_

// Make sure you change these together
#define MCLK_CALB CALBC1_8MHZ
#define MCLOCK_RATE  (8000000)

// Make sure you change these together
#define SMCLK_CALB CALDCO_8MHZ
#define SMCLOCK_RATE (8000000)

#define SPI_CLOCK_RATE (8000000)
#define UART_BAUD_RATE (115200)

#endif /* CONFIG_HPP_ */

/*
 * uart.hpp
 *
 *  Created on: 5 Jun 2021
 *      Author: James Wyatt
 */

#ifndef UART_HPP_
#define UART_HPP_

#include <stdint.h>

namespace uart {


void Init();
void transmit(uint8_t value);
uint8_t receive();

// helpers, header defined so they can be inline'd better??

inline void newline() {
    transmit('\r');
    transmit('\n');
}

/**
 * Write unbounded character array, terminated by '\0'
 */
inline void write(const char* data) {
    unsigned int i = 0;
    while(data[i] != '\0') {
        transmit(data[i]);
        ++i;
    }
}

/**
 * Write bounded char array
 */
inline void write(const char* data, uint8_t count) {
    for(uint8_t i = 0; i < count; i++) {
        transmit(data[i]);
    }
}

template<typename T, unsigned int bufsize>
inline void writeInt(T value) {
    char buf[bufsize];
    char *str = &buf[bufsize-1];
    *str = '\0';
    do {
        T m = value;
        value /= 10;
        char c = (m - 10 * value) + '0';
        *--str = c;
    } while(value);
    if(value < 0) {
        *--str = '-';
    }
    write(str);
}

// 255 = 3 + '\0'
inline void writeUint8(uint8_t v) {
    writeInt<uint8_t, 4>(v);
}

// 65535 = 5 + '\0'
inline void writeUint16(uint16_t v) {
    writeInt<uint16_t, 6>(v);
}

// 4294967295 = 10 + '\0'
inline void writeUint32(uint32_t v) {
    writeInt<uint32_t, 11>(v);
}

// -127 = 4 + '\0'
inline void writeInt8(int8_t v) {
    writeInt<int8_t, 5>(v);
}

// -32768 = 6 + '\0'
inline void writeInt16(int16_t v) {
    writeInt<int16_t, 7>(v);
}

// -2147483648 = 11 + '\0'
inline void writeInt32(int32_t v) {
    writeInt<int32_t, 12>(v);
}

void flush_tx();
void OnTxISR();
void OnRxISR();

} // end namespace uart



#endif /* UART_HPP_ */

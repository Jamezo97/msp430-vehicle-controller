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
uint8_t countrx();
uint8_t counttx();
void delay_cycles(unsigned int count);
bool read(uint8_t& result);

void clear_rx();
void clear_tx();

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

/**
 * Write uint8 byte as 2-char hex
 */
inline void writeHex(uint8_t data) {
    const char* LOOKUP = "0123456789abcdef";
    char a = LOOKUP[static_cast<uint8_t>(data & 0xF)];
    char b = LOOKUP[static_cast<uint8_t>((data >> 4) & 0xF)];
    transmit(b);
    transmit(a);
}

inline void writeHexArray(uint8_t* data, unsigned int len) {
    for(unsigned int idx = 0; idx < len; idx++) {
        writeHex(data[idx]);
    }
}

inline void writeKeyValue(const char* name, uint8_t* data, uint8_t len, bool flip=false) {
    write(name);
    write(": 0x");
    if(flip) {
        for(uint8_t idx = len; idx > 0; idx--) {
            writeHex(data[idx-1]);
        }
    } else {
        for(uint8_t idx = 0; idx < len; idx++) {
            writeHex(data[idx]);
        }
    }
    newline();
}

inline void writeKeyValue(const char* name, uint8_t data) {
    writeKeyValue(name, &data, 1);
}

inline void writeKeyValue(const char* name, uint16_t data) {
    writeKeyValue(name, (uint8_t*)&data, 2, true);
}

inline void writeKeyValue(const char* name, uint32_t data) {
    writeKeyValue(name, (uint8_t*)&data, 4, true);
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

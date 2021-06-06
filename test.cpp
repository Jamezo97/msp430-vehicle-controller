
#include "lib/shift_register.hpp"
#include <stdint.h>
#include <iostream>

int main(int argc, char** argv) {
    ShiftRegister<uint8_t, 2, uint8_t> data;

    std::cout << sizeof(data) << std::endl;

    uint8_t success;

    success = data.shift(1); std::cout << "1=" << +success << std::endl;
    success = data.shift(2); std::cout << "1=" << +success << std::endl;
    success = data.shift(3); std::cout << "1=" << +success << std::endl;
    success = data.shift(4); std::cout << "1=" << +success << std::endl;
    success = data.shift(5); std::cout << "0=" << +success << std::endl;

    std::cout << data.full() << ", " << data.empty() << std::endl;

    uint8_t result;

    success = data.unshift(result); std::cout << "1=" << +success << ", 1=" << +result << std::endl;
    success = data.unshift(result); std::cout << "1=" << +success << ", 2=" << +result << std::endl;
    success = data.unshift(result); std::cout << "1=" << +success << ", 3=" << +result << std::endl;
    success = data.unshift(result); std::cout << "1=" << +success << ", 4=" << +result << std::endl;
    success = data.unshift(result); std::cout << "0=" << +success << ", 4=" << +result << std::endl;
    
    std::cout << data.full() << ", " << data.empty() << std::endl;
}

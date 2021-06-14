
#include "Controller/lib/shift_register.hpp"
#include "Controller/can_types.hpp"
#include <stdint.h>
#include <iostream>

void test_shift() {
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

int main(int argc, char** argv) {
    CANMSG msg;
    uint8_t tmp[CANMSG::SIZE];
    CANMSG result;

    msg.dlc = 5;
    msg.id.EID = 0b100001000000010001;
    msg.id.SID = 0b11111111111;
    msg.id.SRR = 0b0;
    msg.id.IDE = 0b1;

    msg.marshal(tmp);
    result.unmarshal(tmp);

    std::cout << "Equal? " << (msg == result) << std::endl;

    // std::cout << "EID: " << id.EID << " == " << result.EID << std::endl;
    // std::cout << "SID: " << id.SID << " == " << result.SID << std::endl;
    // std::cout << "SRR: " << +id.SRR << " == " << +result.SRR << std::endl;
    // std::cout << "IDE: " << +id.IDE << " == " << +result.IDE << std::endl;
}

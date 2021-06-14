/*
 * can_types.hpp
 *
 *  Created on: 14 Jun 2021
 *      Author: jamez
 */

#ifndef CAN_TYPES_HPP_
#define CAN_TYPES_HPP_

#include <stdint.h>
#include <string.h>

struct CANIdentifier {
    static constexpr size_t SIZE = 4;
    CANIdentifier() : EID(0U), SID(0U), SRR(0U), IDE(0U){}

    uint32_t EID;   // 18 bits Extended identifier bits
    uint16_t SID;   // 11 bits Standard Identifier
    uint8_t SRR;    //  1 bit  For RX buffer, 1 if was remote transmit request
    uint8_t IDE;    //  1 bit  0 = Standard Frame, 1 = Extended frame, to filter, or has been received

    void marshal(uint8_t* result) const {
        result[0] = ((SID >> 3) & 0xFF);
        result[1] = ((SID << 5) & 0xE0) | ((SRR << 4) & 0x10) | ((IDE << 3) & 0x08) | ((EID >> 16) & 0x02);
        result[2] = ((EID >> 8) & 0xFF);
        result[3] = ((EID) & 0xFF);
    }

    void unmarshal(const uint8_t* const data) {
        SID = (data[0] << 3) | ((data[1] >> 5) & 0x7);
        SRR = (data[1] >> 4) & 0x01;
        IDE = (data[1] >> 3) & 0x01;
        EID = (static_cast<uint32_t>(data[1] & 0x03) << static_cast<uint32_t>(16)) | (data[2] << 8) | data[3];
    }

    bool operator==(const CANIdentifier& rhs) const {
        return rhs.SID == SID && rhs.EID == EID && rhs.SRR == SRR && rhs.IDE == IDE;
    }

    void operator=(const unsigned int id) {
        SID = id;
    }
};

struct CANDataLengthContainer {
    static constexpr size_t SIZE = 1;

    CANDataLengthContainer() : RTR(0U), DLC(0U){}
    uint8_t DLC; // data length, value up to 8
    uint8_t RTR; // remote transmission enable bit

    void marshal(uint8_t* result) const {
        result[0] = ((RTR&1) << 6) | (DLC & 0x0F);
    }

    void unmarshal(const uint8_t* const data) {
        RTR = (data[0] >> 6) & 1;
        DLC = (data[0]) & 0x0F;
    }

    bool operator==(const CANDataLengthContainer& rhs) const {
        return rhs.DLC == DLC && rhs.RTR == RTR;
    }

    void operator=(const unsigned int size) {
        DLC = size;
    }
};

struct CANMSG {
    static constexpr size_t SIZE = CANIdentifier::SIZE + CANDataLengthContainer::SIZE + 8;

    CANIdentifier id;
    CANDataLengthContainer dlc;
    uint8_t data[8];

    void marshal(uint8_t* result) {
        id.marshal(result);
        dlc.marshal(result+4);
        memcpy(result+5, data, 8);
    }
    
    void unmarshal(uint8_t* result) {
        id.unmarshal(result);
        dlc.unmarshal(result+4);
        memcpy(data, result+5, 8);
    }

    bool operator==(const CANMSG& rhs) {
        return rhs.id == id && rhs.dlc == dlc && (memcmp(data, rhs.data, 8) == 0);
    }

};

struct CANInterruptStatus {
    uint8_t RX0IFG : 1;
    uint8_t RX1IFG : 1;
    uint8_t TX0REQ : 1;
    uint8_t TX0IF : 1;
    uint8_t TX1REQ : 1;
    uint8_t TX1IF : 1;
    uint8_t TX2REQ : 1;
    uint8_t TX2IF : 1;
};

struct CANReceiveStatus {
    uint8_t RXF : 3; // filter match, 0,1,2,3,4,5, (6,7 are rollover)
    uint8_t MSG : 2; // message type received, 0,1,2,3 are (standard, standard-remote, extended, extended-remote), for buffer 0
    uint8_t RXB0 : 1; // filled RX buffer 0
    uint8_t RXB1 : 1; // filled RX buffer 1
};

#endif /* CAN_TYPES_HPP_ */

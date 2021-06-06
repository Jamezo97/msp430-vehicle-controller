/*
 * shift_register.hpp
 *
 *  Created on: 30 May 2021
 *      Author: James Wyatt
 */

#ifndef LIB_SHIFT_REGISTER_HPP_
#define LIB_SHIFT_REGISTER_HPP_

#include <stdint.h>

/**
 * These shift registers are designed to be 'thread safe' but only over
 * two threads, i.e. on one side we can shift, on the other we can un-shift
 * but if both sides are shifting or un-shifting, then it will cause issues :(
 *
 * You can use this on the main thread and in ISR's. For example, a UART buffer.
 * To transmit, shift the bytes into the buffer and enable UART ISR's
 *  In the UART ISR, un-shift the byte and fill the TX buffer again.
 *
 * This is thread-safe (well, ISR safe) by:
 * - Having 2 pointers (head and tail) which are updated independently
 * - Having a `size` variable. The size could be determined from head-tail
 *      but this would be slightly slower, and shifting/un-shifting will affect
 *      the opposing functionality
 *
 * Note that the member variables are volatile
 * If these are not volatile, the optimizer will optimize away the internal logic!
 * As it cannot understand that ISR's are modifying the buffers too. That was a hard bug to find :P
 */
template<typename T, unsigned int po2_capacity, typename I>
class ShiftRegister {
public:
    
    static const I capacity = 1 << po2_capacity;

    ShiftRegister() : mHead(0U), mTail(0U), mSize(0U) {

    }

    inline void clear() {
        // assume we clear RX buffers, hence the `shift` might occur synchronously
        // this is as safe as I can think of
        mHead = 0; // single op to clear this buffer
        mTail = 0; // this is safe anyway
        mSize = mHead; // if mHead has moved since we changed it, then size is updated?
    }

    inline void seek(I count) {
        if(count < mSize) {
            mSize -= count;
            mTail = mod(mTail + count);
        } else {
            mTail = mHead;
            mSize = 0;
        }
    }

    /**
     * Attempt to put a new value into the shift register
     * Returns true if the data was copied in
     * Returns false if the buffer is full
     */
    inline bool shift(T value) {
        if(!full()) {
            mBuffer[mHead] = value;
            mHead = mod(mHead + 1);
            ++mSize; // increment the size counter last
            return true;
        } else {
            return false;
        }
    }

    /**
     * Attempts to remove a value from the shift register
     * Returns true if data was copied into the provided reference argument
     * Returns false if the buffer is empty
     */
    inline bool unshift(T& result) {
        if(!empty()) {
            result = mBuffer[mTail];
            mTail = mod(mTail + 1);
            mSize--;
            return true;
        } else {
            return false;
        }
    }

    inline bool full() {
        return mSize & capacity;
    }

    inline bool empty() {
        return mSize == 0U;
    }

    inline I size() {
        return mSize;
    }

private:
    static const I ModBits = capacity - 1;
    volatile I mHead;
    volatile I mTail;
    volatile I mSize;
    T mBuffer[capacity];

    static inline I mod(I index) {
        return index & ModBits;
    }
};



#endif /* LIB_SHIFT_REGISTER_HPP_ */

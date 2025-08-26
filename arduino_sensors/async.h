#ifndef NOSYNCCHANNEL_H
#define NOSYNCCHANNEL_H

#include <Arduino.h>

template<typename T>
class NoSyncChannel {
public:
    void setValue(const T& value) {
        mValueToWrite = value;
    }

    bool write() {
        if (mDataReady) {
            return false;
        } else {
            mValue = mValueToWrite;
            mDataReady = true;
            return true;
        }
    }

    bool write(const T& value) {
        if (mDataReady) {
            return false;
        } else {
            mValue = value;
            mDataReady = true;
            return true;
        }
    }

    bool read(T& value) {
        if (mDataReady) {
            value = mValue;
            mDataReady = false;
            return true;
        } else {
            return false;
        }
    }

private:
    T mValue;
    T mValueToWrite;
    bool mDataReady = false;
};

struct Message {
    static uint8_t const kStatusOk = 0;
    static uint8_t const kStatusError = 1;

    uint8_t status;
    int value;
};

#endif

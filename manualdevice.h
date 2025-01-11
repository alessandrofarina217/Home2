#ifndef MANUAL_DEVICE_H
#define MANUAL_DEVICE_H

#include "device.h"

class ManualDevice : public Device {
public:
    void powerOn() override {
        //implementazione per ManualDevice
    }

    void powerOff() override {
        //implementazione per ManualDevice
    }
};

#endif // MANUAL_DEVICE_H

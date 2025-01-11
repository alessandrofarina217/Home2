#ifndef DEVICE_H
#define DEVICE_H

#include <string>

class Device {
public:
    //funzioni virtuali pure
    virtual void powerOn() = 0;
    virtual void powerOff() = 0;

    //distruttore virtuale
    virtual ~Device() {}

protected:
    std::string name;
    std::string id;
    double tdp; //potenza
    double prt; //priorita'
    bool status = false; //stato: true = acceso | false = spento
};

#endif // DEVICE_H

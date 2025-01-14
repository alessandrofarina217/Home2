#ifndef DEVICE_H
#define DEVICE_H

#include <string>

class Device {
protected:
    std::string name;
    std::string id;
    double power;        // in kW, negative for consumption, positive for production
    bool isOn;
    int priority;        // Higher number = higher priority
    
public:
    Device(const std::string& name, const std::string& id, double power, int priority)
        : name(name), id(id), power(power), isOn(false), priority(priority) {}
    
    virtual ~Device() = default;
    
    // Pure virtual functions that derived classes must implement
    virtual void turnOn() = 0;
    virtual void turnOff() = 0;
    virtual bool canBeTurnedOff() const = 0;  // Some devices might not be allowed to turn off (e.g., fridge)
    
    // Common functionality for all devices
    std::string getName() const { return name; }
    std::string getId() const { return id; }
    double getPower() const { return power; }
    bool isActive() const { return isOn; }
    int getPriority() const { return priority; }
    
    // Calculate energy consumption from start to end time (in minutes)
    double calculateEnergy(int minutes) const {
        return isOn ? (power * minutes / 60.0) : 0.0;  // Convert minutes to hours
    }
    
    // Virtual function that can be overridden if needed
    virtual bool needsAutomaticShutdown() const {
        return false;  // By default, devices don't need automatic shutdown
    }
};

#endif // DEVICE_H

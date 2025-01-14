#ifndef DERIVED_DEVICES_H
#define DERIVED_DEVICES_H

#include "device.h"

class ManualDevice : public Device {
private:
    bool canBeForceOff;  // Indica se il dispositivo può essere forzatamente spento (es: frigorifero non può)

public:
    ManualDevice(const std::string& name, const std::string& id, double power, 
                int priority, bool canBeForceOff = true)
        : Device(name, id, power, priority), canBeForceOff(canBeForceOff) {}

    void turnOn() override {
        isOn = true;
    }

    void turnOff() override {
        isOn = false;
    }

    bool canBeTurnedOff() const override {
        return canBeForceOff;
    }
};

class AutoDevice : public Device {
private:
    int durationMinutes;     // Durata del ciclo in minuti
    int startTimeMinute;     // Minuto di inizio del ciclo corrente
    bool cycleInProgress;    // Indica se un ciclo è in corso

public:
    AutoDevice(const std::string& name, const std::string& id, double power,
              int priority, int durationMinutes)
        : Device(name, id, power, priority), 
          durationMinutes(durationMinutes),
          startTimeMinute(0),
          cycleInProgress(false) {}

    void turnOn() override {
        if (!cycleInProgress) {
            isOn = true;
            cycleInProgress = true;
            startTimeMinute = 0;  // Questo verrà settato dal DeviceManager con l'ora corrente
        }
    }

    void turnOff() override {
        isOn = false;
        cycleInProgress = false;
    }

    bool canBeTurnedOff() const override {
        return true;  // I dispositivi a ciclo prefissato possono sempre essere spenti
    }

    bool needsAutomaticShutdown() const override {
        return true;
    }

    // Metodi specifici per i dispositivi automatici
    int getDuration() const {
        return durationMinutes;
    }

    void setStartTime(int currentMinute) {
        startTimeMinute = currentMinute;
    }

    bool shouldTurnOff(int currentMinute) const {
        return cycleInProgress && 
               (currentMinute - startTimeMinute >= durationMinutes);
    }

    int getEndTime() const {
        return startTimeMinute + durationMinutes;
    }
};

#endif // DERIVED_DEVICES_H

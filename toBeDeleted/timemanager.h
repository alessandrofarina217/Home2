#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include "device_manager.h"

class TimeManager {
private:
    int currentMinutes;  // Minuti trascorsi dalla mezzanotte
    DeviceManager& deviceManager;
    
    // Converti una stringa orario (HH:MM) in minuti
    static int timeStringToMinutes(const std::string& timeStr) {
        int hours, minutes;
        char delimiter;
        std::istringstream ss(timeStr);
        
        ss >> hours >> delimiter >> minutes;
        
        if (ss.fail() || delimiter != ':' || hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
            throw std::invalid_argument("Invalid time format. Use HH:MM (24h format)");
        }
        
        return hours * 60 + minutes;
    }
    
    // Converti minuti in stringa orario (HH:MM)
    static std::string minutesToTimeString(int minutes) {
        int hours = minutes / 60;
        int mins = minutes % 60;
        
        std::ostringstream ss;
        ss << std::setfill('0') << std::setw(2) << hours << ":" 
           << std::setfill('0') << std::setw(2) << mins;
        
        return ss.str();
    }

    // Verifica se il nuovo orario è valido
    bool isValidNewTime(int newTimeMinutes) const {
        // L'orario deve essere nel futuro e non oltre le 23:59
        return newTimeMinutes > currentMinutes && newTimeMinutes < 24 * 60;
    }

public:
    TimeManager(DeviceManager& dm) 
        : currentMinutes(0), deviceManager(dm) {}
    
    // Ottieni l'orario corrente come stringa
    std::string getCurrentTime() const {
        return minutesToTimeString(currentMinutes);
    }
    
    // Ottieni i minuti correnti dalla mezzanotte
    int getCurrentMinutes() const {
        return currentMinutes;
    }
    
    // Imposta un nuovo orario e simula il passaggio del tempo
    void setTime(const std::string& newTime) {
        int newTimeMinutes = timeStringToMinutes(newTime);
        
        if (!isValidNewTime(newTimeMinutes)) {
            throw std::invalid_argument("New time must be in the future and before 23:59");
        }
        
        // Simula il passaggio del tempo minuto per minuto
        while (currentMinutes < newTimeMinutes) {
            currentMinutes++;
            deviceManager.checkAndUpdateDevices(currentMinutes);
        }
    }
    
    // Resetta il tempo a 00:00
    void resetTime() {
        currentMinutes = 0;
    }
    
    // Converti una stringa orario in minuti (metodo pubblico per uso esterno)
    static int convertTimeToMinutes(const std::string& timeStr) {
        return timeStringToMinutes(timeStr);
    }
    
    // Verifica se un orario è nel formato corretto
    static bool isValidTimeFormat(const std::string& timeStr) {
        try {
            timeStringToMinutes(timeStr);
            return true;
        } catch (const std::invalid_argument&) {
            return false;
        }
    }
};

#endif // TIME_MANAGER_H

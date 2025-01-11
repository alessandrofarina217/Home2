#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <map>
#include <list>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include "derived_devices.h"

struct Timer {
    std::string deviceId;
    int startTimeMinutes;  // Tempo di accensione in minuti dalla mezzanotte
    int stopTimeMinutes;   // Tempo di spegnimento in minuti dalla mezzanotte (opzionale per AutoDevice)
    bool isValid;          // Flag per indicare se il timer è ancora valido

    Timer(const std::string& id, int start, int stop = -1)
        : deviceId(id), startTimeMinutes(start), stopTimeMinutes(stop), isValid(true) {}
};

class DeviceManager {
private:
    // Costanti
    const double MAX_POWER_FROM_GRID;  // Potenza massima dalla rete (3.5 kW)
    
    // Contenitori principali
    std::map<std::string, std::shared_ptr<Device>> devices;              // Tutti i dispositivi per ID
    std::multimap<int, std::shared_ptr<Device>> activeDevices;          // Dispositivi attivi ordinati per priorità
    std::list<Timer> timers;                                            // Lista dei timer
    
    // Metodi privati di utility
    double calculateTotalPower() const {
        double total = 0.0;
        for (const auto& [priority, device] : activeDevices) {
            total += device->getPower();
        }
        return total;
    }

    void enforceMaxPowerPolicy() {
        double totalPower = calculateTotalPower();
        double maxAllowedPower = MAX_POWER_FROM_GRID;

        // Aggiungi potenza dal fotovoltaico se presente e attivo
        auto it = devices.find("fotovoltaico");
        if (it != devices.end() && it->second->isActive()) {
            maxAllowedPower += std::abs(it->second->getPower());
        }

        // Se la potenza totale supera il massimo, spegni i dispositivi in ordine
        while (totalPower < -maxAllowedPower) {  // Nota: i consumi sono negativi
            // Trova il dispositivo con priorità più bassa che può essere spento
            auto it = std::find_if(activeDevices.begin(), activeDevices.end(),
                [](const auto& pair) { return pair.second->canBeTurnedOff(); });

            if (it == activeDevices.end()) {
                throw std::runtime_error("Impossibile rispettare il limite di potenza!");
            }

            it->second->turnOff();
            totalPower -= it->second->getPower();
            activeDevices.erase(it);
        }
    }

public:
    explicit DeviceManager(double maxPower = 3.5) : MAX_POWER_FROM_GRID(maxPower) {}

    // Gestione dispositivi
    void addDevice(std::shared_ptr<Device> device) {
        if (devices.find(device->getId()) != devices.end()) {
            throw std::invalid_argument("Device ID already exists");
        }
        devices[device->getId()] = device;
    }

    void removeDevice(const std::string& id) {
        auto it = devices.find(id);
        if (it != devices.end()) {
            if (it->second->isActive()) {
                // Rimuovi dai dispositivi attivi se necessario
                for (auto activeIt = activeDevices.begin(); activeIt != activeDevices.end(); ++activeIt) {
                    if (activeIt->second->getId() == id) {
                        activeDevices.erase(activeIt);
                        break;
                    }
                }
            }
            devices.erase(it);
        }
    }

    // Gestione stati dei dispositivi
    void turnOnDevice(const std::string& id, int currentTimeMinutes) {
        auto it = devices.find(id);
        if (it == devices.end()) {
            throw std::invalid_argument("Device not found");
        }

        auto device = it->second;
        if (!device->isActive()) {
            device->turnOn();
            
            // Per dispositivi automatici, imposta il tempo di inizio
            if (auto autoDevice = std::dynamic_pointer_cast<AutoDevice>(device)) {
                autoDevice->setStartTime(currentTimeMinutes);
            }

            activeDevices.insert({device->getPriority(), device});
            enforceMaxPowerPolicy();
        }
    }

    void turnOffDevice(const std::string& id) {
        auto it = devices.find(id);
        if (it != devices.end() && it->second->isActive()) {
            it->second->turnOff();
            
            // Rimuovi dai dispositivi attivi
            for (auto activeIt = activeDevices.begin(); activeIt != activeDevices.end(); ++activeIt) {
                if (activeIt->second->getId() == id) {
                    activeDevices.erase(activeIt);
                    break;
                }
            }
        }
    }

    // Gestione timer
    void addTimer(const std::string& deviceId, int startTime, int stopTime = -1) {
        auto it = devices.find(deviceId);
        if (it == devices.end()) {
            throw std::invalid_argument("Device not found");
        }

        // Rimuovi eventuali timer esistenti per questo dispositivo
        removeTimer(deviceId);
        
        // Aggiungi il nuovo timer
        timers.emplace_back(deviceId, startTime, stopTime);
    }

    void removeTimer(const std::string& deviceId) {
        timers.remove_if([deviceId](const Timer& timer) {
            return timer.deviceId == deviceId;
        });
    }

    // Metodi per il monitoraggio e la gestione del tempo
    void checkAndUpdateDevices(int currentTimeMinutes) {
        // Controlla i timer
        for (auto& timer : timers) {
            if (!timer.isValid) continue;

            auto device = devices[timer.deviceId];
            
            // Gestisci accensione
            if (currentTimeMinutes == timer.startTimeMinutes && !device->isActive()) {
                turnOnDevice(timer.deviceId, currentTimeMinutes);
            }
            
            // Gestisci spegnimento per dispositivi manuali
            if (timer.stopTimeMinutes != -1 && 
                currentTimeMinutes == timer.stopTimeMinutes && 
                device->isActive()) {
                turnOffDevice(timer.deviceId);
            }
        }

        // Controlla i dispositivi automatici per lo spegnimento
        for (auto it = activeDevices.begin(); it != activeDevices.end();) {
            if (auto autoDevice = std::dynamic_pointer_cast<AutoDevice>(it->second)) {
                if (autoDevice->shouldTurnOff(currentTimeMinutes)) {
                    turnOffDevice(autoDevice->getId());
                    it = activeDevices.begin(); // Reset iterator after modification
                } else {
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }

    // Metodi per il reporting
    double getDeviceEnergy(const std::string& id, int totalMinutes) const {
        auto it = devices.find(id);
        if (it != devices.end()) {
            return it->second->calculateEnergy(totalMinutes);
        }
        return 0.0;
    }

    std::vector<std::pair<std::string, double>> getAllDevicesEnergy(int totalMinutes) const {
        std::vector<std::pair<std::string, double>> result;
        for (const auto& [id, device] : devices) {
            result.emplace_back(id, device->calculateEnergy(totalMinutes));
        }
        return result;
    }

    bool isDeviceActive(const std::string& id) const {
        auto it = devices.find(id);
        return it != devices.end() && it->second->isActive();
    }
};

#endif // DEVICE_MANAGER_H

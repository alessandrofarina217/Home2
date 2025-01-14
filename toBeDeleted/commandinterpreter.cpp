// command_interpreter.cpp
#include "command_interpreter.h"
#include <sstream>

CommandInterpreter::CommandInterpreter(TimeManager& tm, DeviceManager& dm) 
    : timeManager(tm), deviceManager(dm) {
    initializePatterns();
}

std::vector<std::string> CommandInterpreter::tokenize(const std::string& command) {
    std::vector<std::string> tokens;
    std::istringstream iss(command);
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

CommandInterpreter::CommandPattern CommandInterpreter::findMatchingPattern(
    const std::vector<std::string>& tokens) {
    
    for (const auto& [pattern, cmdPattern] : commandPatterns) {
        std::vector<std::string> patternTokens = tokenize(pattern);
        
        if (tokens.size() == patternTokens.size()) {
            bool match = true;
            for (size_t i = 0; i < tokens.size(); i++) {
                if (patternTokens[i].starts_with("${") && patternTokens[i].ends_with("}")) {
                    continue; // Questo è un placeholder, accetta qualsiasi token
                }
                if (patternTokens[i] != tokens[i]) {
                    match = false;
                    break;
                }
            }
            if (match) return cmdPattern;
        }
    }
    return CommandPattern{"", 0, {}, CommandType::INVALID, nullptr};
}

void CommandInterpreter::initializePatterns() {
    commandPatterns["set ${DEVICENAME} on"] = {
        "set ${DEVICENAME} on",
        1,
        {"device"},
        CommandType::SET_DEVICE_ON,
        [this](const auto& params) { 
            return deviceManager.isDeviceActive(params[0]); 
        }
    };

    commandPatterns["set ${DEVICENAME} off"] = {
        "set ${DEVICENAME} off",
        1,
        {"device"},
        CommandType::SET_DEVICE_OFF,
        [this](const auto& params) { 
            return deviceManager.isDeviceActive(params[0]); 
        }
    };

    commandPatterns["set ${DEVICENAME} ${START} ${STOP}"] = {
        "set ${DEVICENAME} ${START} ${STOP}",
        3,
        {"device", "time", "time"},
        CommandType::SET_DEVICE_TIMER,
        [](const auto& params) { 
            return TimeManager::isValidTimeFormat(params[1]) && 
                   TimeManager::isValidTimeFormat(params[2]); 
        }
    };

    commandPatterns["rm ${DEVICENAME}"] = {
        "rm ${DEVICENAME}",
        1,
        {"device"},
        CommandType::REMOVE_TIMER,
        [this](const auto& params) { 
            return deviceManager.isDeviceActive(params[0]); 
        }
    };

    commandPatterns["show"] = {
        "show",
        0,
        {},
        CommandType::SHOW_ALL,
        nullptr  // Nessuna validazione necessaria
    };

    commandPatterns["show ${DEVICENAME}"] = {
        "show ${DEVICENAME}",
        1,
        {"device"},
        CommandType::SHOW_DEVICE,
        [this](const auto& params) { 
            return deviceManager.isDeviceActive(params[0]); 
        }
    };

    commandPatterns["set time ${TIME}"] = {
        "set time ${TIME}",
        1,
        {"time"},
        CommandType::SET_TIME,
        [](const auto& params) { 
            return TimeManager::isValidTimeFormat(params[0]); 
        }
    };

    commandPatterns["reset time"] = {
        "reset time",
        0,
        {},
        CommandType::RESET_TIME,
        nullptr
    };

    commandPatterns["reset timers"] = {
        "reset timers",
        0,
        {},
        CommandType::RESET_TIMERS,
        nullptr
    };

    commandPatterns["reset all"] = {
        "reset all",
        0,
        {},
        CommandType::RESET_ALL,
        nullptr
    };
}

CommandInterpreter::CommandResult CommandInterpreter::interpretCommand(
    const std::string& commandStr) {
    
    auto tokens = tokenize(commandStr);
    if (tokens.empty()) {
        return {CommandType::INVALID, {}, false, "Empty command"};
    }

    auto pattern = findMatchingPattern(tokens);
    if (pattern.type == CommandType::INVALID) {
        return {CommandType::INVALID, {}, false, "Invalid command pattern"};
    }

    // Estrai i parametri
    std::vector<std::string> params;
    auto patternTokens = tokenize(pattern.pattern);
    for (size_t i = 0; i < tokens.size(); i++) {
        if (patternTokens[i].starts_with("${") && patternTokens[i].ends_with("}")) {
            params.push_back(tokens[i]);
        }
    }

    // Valida i parametri
    if (pattern.validator && !pattern.validator(params)) {
        return {pattern.type, params, false, "Invalid parameters"};
    }

    return {pattern.type, params, true, ""};
}

bool CommandInterpreter::executeCommand(const CommandResult& result) {
    if (!result.isValid) return false;

    try {
        switch (result.type) {
            case CommandType::SET_DEVICE_ON:
                deviceManager.turnOnDevice(result.parameters[0], 
                                        timeManager.getCurrentMinutes());
                break;
                
            case CommandType::SET_DEVICE_OFF:
                deviceManager.turnOffDevice(result.parameters[0]);
                break;
                
            case CommandType::SET_DEVICE_TIMER:
                deviceManager.addTimer(
                    result.parameters[0],
                    TimeManager::convertTimeToMinutes(result.parameters[1]),
                    TimeManager::convertTimeToMinutes(result.parameters[2])
                );
                break;
                
            case CommandType::REMOVE_TIMER:
                deviceManager.removeTimer(result.parameters[0]);
                break;
                
            case CommandType::SHOW_ALL:
                // TODO: Implementare visualizzazione di tutti i dispositivi
                break;
                
            case CommandType::SHOW_DEVICE:
                // TODO: Implementare visualizzazione singolo dispositivo
                break;
                
            case CommandType::SET_TIME:
                timeManager.setTime(result.parameters[0]);
                break;
                
            case CommandType::RESET_TIME:
                timeManager.resetTime();
                break;
                
            case CommandType::RESET_TIMERS:
                // TODO: Implementare reset timer
                break;
                
            case CommandType::RESET_ALL:
                // TODO: Implementare reset completo
                break;
                
            default:
                return false;
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// command_interpreter.h
#ifndef COMMAND_INTERPRETER_H
#define COMMAND_INTERPRETER_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include "time_manager.h"
#include "device_manager.h"

class CommandInterpreter {
public:
    enum class CommandType {
        SET_DEVICE_ON,
        SET_DEVICE_OFF,
        SET_DEVICE_TIMER,
        REMOVE_TIMER,
        SHOW_ALL,
        SHOW_DEVICE,
        SET_TIME,
        RESET_TIME,
        RESET_TIMERS,
        RESET_ALL,
        INVALID
    };

    struct CommandResult {
        CommandType type;
        std::vector<std::string> parameters;
        bool isValid;
        std::string errorMessage;
    };

    CommandInterpreter(TimeManager& tm, DeviceManager& dm);
    
    CommandResult interpretCommand(const std::string& commandStr);
    bool executeCommand(const CommandResult& result);

private:
    struct CommandPattern {
        std::string pattern;
        int expectedParams;
        std::vector<std::string> paramTypes;
        CommandType type;
        std::function<bool(const std::vector<std::string>&)> validator;
    };

    TimeManager& timeManager;
    DeviceManager& deviceManager;
    std::map<std::string, CommandPattern> commandPatterns;

    std::vector<std::string> tokenize(const std::string& command);
    CommandPattern findMatchingPattern(const std::vector<std::string>& tokens);
    void initializePatterns();
};

#endif // COMMAND_INTERPRETER_H

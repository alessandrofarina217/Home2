#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <memory>
#include <algorithm>

// Forward declaration
class CommandParser;

// Base class for all commands
class Command {
public:
    virtual ~Command() = default;
    virtual bool execute(const std::vector<std::string>& args) = 0;
    virtual std::string getUsage() const = 0;
};

// Example command implementation
class EchoCommand : public Command {
public:
    bool execute(const std::vector<std::string>& args) override {
        for (size_t i = 0; i < args.size(); ++i) {
            std::cout << args[i];
            if (i < args.size() - 1) std::cout << " ";
        }
        std::cout << std::endl;
        return true;
    }
    
    std::string getUsage() const override {
        return "echo [message...]";
    }
};

// Helper function to replace std::make_unique (C++14)
template<typename T, typename... Args>
std::unique_ptr<T> makeUnique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class CommandParser {
private:
    // Map of command names to command objects
    std::map<std::string, std::unique_ptr<Command>> commands;
    
    // Split input string into tokens
    std::vector<std::string> tokenize(const std::string& input) {
        std::vector<std::string> tokens;
        std::stringstream ss(input);
        std::string token;
        
        while (ss >> token) {
            // Handle quoted strings
            if (!token.empty() && token.front() == '"' && token.back() != '"') {
                std::string quoted = token;
                while (ss >> token) {
                    quoted += " " + token;
                    if (!token.empty() && token.back() == '"') break;
                }
                // Remove quotes and add to tokens
                if (quoted.length() >= 2) {
                    quoted = quoted.substr(1, quoted.length() - 2);
                    tokens.push_back(quoted);
                }
            } else if (!token.empty() && token.front() == '"' && token.back() == '"') {
                // Handle single quoted token
                token = token.substr(1, token.length() - 2);
                tokens.push_back(token);
            } else {
                tokens.push_back(token);
            }
        }
        return tokens;
    }

public:
    // Register a new command
    void registerCommand(const std::string& name, std::unique_ptr<Command> cmd) {
        commands[name] = std::move(cmd);
    }
    
    // Process user input
    bool processInput(const std::string& input) {
        auto tokens = tokenize(input);
        
        if (tokens.empty()) {
            return true;
        }
        
        std::string commandName = tokens[0];
        // Convert command to lowercase for case-insensitive comparison
        std::transform(commandName.begin(), commandName.end(), 
                      commandName.begin(), ::tolower);
        
        // Check if command exists
        auto it = commands.find(commandName);
        if (it == commands.end()) {
            std::cout << "Unknown command: " << commandName << std::endl;
            return false;
        }
        
        // Remove command name from arguments
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());
        
        // Execute command
        return it->second->execute(args);
    }
    
    // Display available commands
    void showHelp() const {
        std::cout << "Available commands:\n";
        for (const auto& cmd : commands) {
            std::cout << cmd.first << " - Usage: " << cmd.second->getUsage() << std::endl;
        }
    }
};

// Example calculator command
class CalculateCommand : public Command {
public:
    bool execute(const std::vector<std::string>& args) override {
        if (args.size() < 3) {
            std::cout << "Error: Not enough arguments for calculation\n";
            std::cout << "Usage: " << getUsage() << std::endl;
            return false;
        }

        std::string operation = args[0];
        
        try {
            double result = std::stod(args[1]);
            
            if (operation == "add") {
                for (size_t i = 2; i < args.size(); i++) {
                    result += std::stod(args[i]);
                }
                std::cout << "Result: " << result << std::endl;
                return true;
            } else {
                std::cout << "Unknown operation: " << operation << std::endl;
                return false;
            }
        } catch (const std::invalid_argument&) {
            std::cout << "Error: Please provide valid numbers\n";
            return false;
        }
    }
    
    std::string getUsage() const override {
        return "calc add <number1> <number2> [more numbers...]";
    }
};

int main() {
    CommandParser parser;
    
    // Register commands using our makeUnique helper instead of std::make_unique
    parser.registerCommand("echo", makeUnique<EchoCommand>());
    parser.registerCommand("calc", makeUnique<CalculateCommand>());
    
    std::string input;
    
    std::cout << "Welcome to the terminal! Type 'exit' to quit.\n";
    
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        
        if (input == "exit") {
            break;
        } else if (input == "help") {
            parser.showHelp();
        } else {
            parser.processInput(input);
        }
    }
    
    return 0;
}

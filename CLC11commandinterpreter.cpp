//file scritto da Alessandro Farina

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <memory>
#include <algorithm>

// Forward declaration
class CommandParser;

class Command         //classe di base per ciascun comando
{
public:
    virtual ~Command() = default;
    virtual bool execute(const std::vector<std::string>& args) = 0;
};

class setCommand : public Command                 //classe per comando SET
{
public:
    bool execute(const std::vector<std::string>& args) override
    {
        if(args.size() > 3) return false;                                               //l'input non sarà mai corretto 
        else if(args[0] == "time" && args.size() == 2) dm.setTime(args);                //eventualmente si potrebbe cambiare con un set
        else if (dm.checkList(args) && args.size == 2)                                  //controllo nella tabella dei dispositivi
        {
            if(args[1] == "on" || args[1] == "off") dm.setPower(args);
            else dm.setTimer(args);
        }
        else return false;

        return true;
        
    }

};

class rmCommand : public Command                //classe per comando REMOVE
{
public:
    bool execute(const std::vector<std::string>& args) override
    {
        if(dm.checkList(args)) 
        {
            dm.removeTimer(args);                            //se il device è come argomento, rimuove il timer associato
            return true;
        }
        else cout<<"comando inserito non valido."<<endl;    //stampa errore input
    }
};

class showCommand : public Command             //classe per comando SHOW
{
public:
    bool execute(const std::vector<std::string>& args) override 
    {
        if(args.empty()) dm.showConsumption()                        //chiama showConsumption
        else if (dm.checkList(args)) dm.printDeviceConsumption();    //chiama printDeviceConsumption
        else cout<<"comando inserito non valido."<<endl;             //gestione errore in input
    }

};

class resetCommand : public Command         //classe per comando RESET
{
public:
    bool execute(const std::vector<std::string>& args) override 
    {
        if(args.size() == 1)
        {
            if(args[0] == "time") dm.resetTime();                                        //chiama resetTime
            else if(args[0] == "timers") dm.resetTimers();                               //chiama resetTimers
            else dm.resetAll();                                                          //chiama resetAll
        }
    }
}; 

template<typename T, typename... Args>                                        //helper function per utilizzare make unique anche al di fuori di c++14
std::unique_ptr<T> makeUnique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class CommandParser 
{
private:

    std::map<std::string, std::unique_ptr<Command>> commands;        //mappa dei nomi dei comandi
    
    
    std::vector<std::string> tokenize(const std::string& input)     //funzione per creare i token
    {
        std::vector<std::string> tokens;
        std::stringstream stream(input);
        std::string token;
        
        while (stream >> token) 
        {
                tokens.push_back(token);
        }
        return tokens;
    }

public:
    //registrazione dei comandi                                                                    DA CONVERTIRE IN SET FISSO
    void registerCommand(const std::string& name, std::unique_ptr<Command> cmd) 
    {
        commands[name] = std::move(cmd);
    }
    
    // Process user input
    bool processInput(const std::string& input) 
    {
        auto tokens = tokenize(input);
        
        if (tokens.empty()) 
        {
            return true;
        }
        
        std::string commandName = tokens[0];
        
        // Check if command exists
        auto it = commands.find(commandName);
        if (it == commands.end()) 
        {
            std::cout << "Il comando inserito non e' valido. " << commandName << std::endl;
            return false;
        }
        
        //rimozione del comando dal vettore dei token
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());
        
        //passaggio al secondo livello (controllo parametri)
        return it->second->execute(args);
    }
    
};

int main() {
    CommandParser parser;
    
    // Register commands using our makeUnique helper instead of std::make_unique
    parser.registerCommand("echo", makeUnique<EchoCommand>());
    parser.registerCommand("calc", makeUnique<CalculateCommand>());
    
    parser.registerCommand("set", makeUnique<setCommand>());
    parser.registerCommand("rm", makeUnique<rmCommand>());
    parser.registerCommand("show", makeUnique<showCommand>());
    parser.registerCommand("reset", makeUnique<resetCommand>());
    
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

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
    virtual void execute(const std::vector<std::string>& args) = 0;
    void printInvalid() const
    {
        cout<<"Comando inserito non valido, Riprovare.\n";
    }
};

class SetCommand : public Command                 //classe per comando SET
{
public:
    void execute(const std::vector<std::string>& args) override
    {
        if(args.size() > 3) return false;                                               //l'input non sarà mai corretto 
        else if(args[0] == "time" && args.size() == 2) dm.setTime(args);                //eventualmente si potrebbe cambiare con un set
        else if (dm.checkList(args) && args.size == 2)                                  //controllo nella tabella dei dispositivi
        {
            if(args[1] == "on" || args[1] == "off") dm.setPower(args);
            else dm.setTimer(args);
        }
        else printInvalid();
    }

};

class RmCommand : public Command                //classe per comando REMOVE
{
public:
    void execute(const std::vector<std::string>& args) override
    {
        if(dm.checkList(args)) 
        {
            dm.removeTimer(args);                            //se il device è come argomento, rimuove il timer associato
            return;
        }
        else printInvalid();    //stampa errore input
    }
};

class ShowCommand : public Command             //classe per comando SHOW
{
public:
    void execute(const std::vector<std::string>& args) override 
    {
        if(args.empty()) dm.showConsumption()                        //chiama showConsumption
        else if (dm.checkList(args)) dm.printDeviceConsumption();    //chiama printDeviceConsumption
        else printInvalid();            //gestione errore in input
    }

};

class ResetCommand : public Command         //classe per comando RESET
{
public:
    void execute(const std::vector<std::string>& args) override 
    {
        if(args.size() == 1)
        {
            if(args[0] == "time") dm.resetTime();                                        //chiama resetTime
            else if(args[0] == "timers") dm.resetTimers();                               //chiama resetTimers
            else dm.resetAll();                                                          //chiama resetAll
        }
    }
}; 

class CommandParser 
{
private:

    std::unordered_map<std::string, std::unique_ptr<Command>> commands;        //mappa dei nomi dei comandi
    commandMap["set"] = std::unique_ptr<Command>(new SetCommand());
    commandMap["reset"] = std::unique_ptr<Command>(new ResetCommand());
    commandMap["rm"] = std::unique_ptr<Command>(new RmCommand());
    commandMap["show"] = std::unique_ptr<Command>(new ShowCommand());
    
    
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
    void processInput(const std::string& input) 
    {
        auto tokens = tokenize(input);
        
        if (tokens.empty()) 
        {
            cout<<"Inserire un comando!\n";
            return;
        }
        
        std::string commandName = tokens[0];
        
        // Check if command exists
        auto it = commands.find(commandName);
        if (it == commands.end()) 
        {
            std::cout << "Il comando inserito non e' valido. "<<commandName<<std::endl;
            return;
        }
        
        //rimozione del comando dal vettore dei token
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());
        
        //passaggio al secondo livello (controllo parametri)
        it->second->execute(args);
    }
    
};

int main() 
{
    CommandParser parser;
    DeviceManager dm;
    TimeManager tm;
    
    
    std::string input;
    
    std::cout<<"Benvenuti in HomeManager.\n";
    std::cout<<"Per favore, utilizzare le virgolette \"" <<" per indicare i nomi dei dispositivi.\n";            //CONTROLLARE input
    
    do
    {
        std::cout << "comando: > ";
        std::getline(std::cin, input);
        
        if (input == "exit") 
        {
            break;
        }
        else if (input == "help") 
        {
            parser.showHelp();
        } 
        else 
        {
            parser.processInput(input);
        }
    } while(time<=end);
    
    return 0;
}

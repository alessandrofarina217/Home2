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
  void printInvalid() const
    {
        cout<<"Comando inserito non valido, Riprovare.\n";
    }
    virtual int checkArgs(const std::vector<std::string>& args) = 0;

public:
    virtual ~Command() = default;
    virtual void execute(const std::vector<std::string>& args) = 0;
};

class SetCommand : public Command                 //classe per comando SET
{
    int checkArgs(const std::vector<std::string>& args) override 
    {
        
    }
public:
    void execute(const std::vector<std::string>& args) override
    {
        if(args.size() > 3) return;                                               //l'input non sarà mai corretto 
        else if(args[0] == "time" && args.size() == 2) dm.setTime(args);                //eventualmente si potrebbe cambiare con un set
        else if (dm.checkList(args) && args.size == 2)                                  //controllo nella tabella dei dispositivi
        {
            if(args[1] == "on" || args[1] == "off") dm.setPower(args);
            else dm.setTimer(args);
        }
        else printInvalid();
    }

};

class RmCommand : public Command                //classe per comando REMOVE        MODIFICATA
{
    int checkArgs(const std::vector<std::string>& args) override            //member function per verificare gli argomenti e segnalare la funzione corretta da chiamare
    {
        if(dm.checkList(args))    return 1;
        else return -1;
    }
public:
    void execute(const std::vector<std::string>& args) override
    {
        switch(checkArgs(args))
            {
                case 1:
                    dm.removeTimer(args);
                    break;
                case -1:                                            //caso errore
                    printInvalid();
                    break;
            }
    }
};

class ShowCommand : public Command             //classe per comando SHOW        MODIFICATA
{
    int checkArgs(const std::vector<std::string>& args) override 
    {
        if(args.empty()) return 1;
        if(dm:checkList(args)) return 2;
        else return -1;
    }
public:
    void execute(const std::vector<std::string>& args) override 
    {
        switch(checkArgs(args))
            {
                case 1:
                    dm.showConsumption();
                    break;
                case 2:
                    dm.printDeviceConsumption();
                    break;
                case -1;
                    printInvalid();
                    break;
            }
    }

};

class ResetCommand : public Command         //classe per comando RESET            MODIFICATA
{
    int checkArgs(const std::vector<std::string>& args) override 
    {
        if(args.size==1)
        {
            if(args[0]=="time") return 1;
            if(args[0]=="timers") return 2;
            if(args[0]=="all") return 3;
            else return -1;
        }
    }
public:
    void execute(const std::vector<std::string>& args) override 
    {
        switch(checkArgs(args))
            {
                case 1:
                    dm.resetTime();
                    break;
                case 2:
                    dm.resetTimers();
                    break;
                case 3:
                    dm.resetAll();
                    break;
                case -1:
                    printInvalid();
                    break;
            }
    }
}; 

class CommandParser 
{
private:

    std::unordered_map<std::string, std::unique_ptr<Command>> commands;        //mappa dei nomi dei comandi
    
    
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
    CommandParser()
    {
        commands["set"] = std::unique_ptr<Command>(new SetCommand());
        commands["reset"] = std::unique_ptr<Command>(new ResetCommand());
        commands["rm"] = std::unique_ptr<Command>(new RmCommand());
        commands["show"] = std::unique_ptr<Command>(new ShowCommand());
    }
        
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
            std::cout << "Il comando inserito non e' valido: "<<commandName<<std::endl;
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

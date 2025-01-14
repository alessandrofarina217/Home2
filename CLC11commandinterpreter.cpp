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

class SetCommand : public Command                 //classe per comando SET    MODIFICATA
{
    int checkArgs(const std::vector<std::string>& args) override 
    {
        if(args.size() > 3) return -1;
        if(args[0] == "time" && args.size() == 2) return 1;
        if(dm.checkList(args) && args.size == 2)
        {
            if(args[1]=="on" || args[1]=="off") return 2;
            else return 3;
        }
        else return -1;
    }
public:
    void execute(const std::vector<std::string>& args) override
    {
        switch(checkArgs(args))
          {
            case 1:
              dm.setTime(args);
              break;
            case 2:
              dm.setPower(args);
              break;
            case 3:
              dm.setTimer(args);
              break;
            case -1:
              printInvalid();
              break;
          }
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
    
    std::vector<std::string> tokenize(const std::string& input)
{
    bool virgolettato = false;
    int tLength = 0;
    int sIndex = 0;
    std::vector<std::string> tokens;

    for (int i = 0; i < input.length(); i++)
    {
        if (!virgolettato && input[i] == ' ')  //fuori dalle virgolette spezza allo spazio
        {
            if (tLength > 0) 
            {
                tokens.push_back(input.substr(sIndex, tLength));
                tLength = 0;
            }
            sIndex = i + 1;
        }
        else if (input[i] == '"')  //gestione delle virgolette
        {
            virgolettato = !virgolettato;
            if (virgolettato) 
            {
                sIndex = i + 1;  //inizia dopo l'apertura delle virgolette
            }
            else 
            {
                tokens.push_back(input.substr(sIndex, i - sIndex));
                tLength = 0;
                sIndex = i + 1;
            }
        }
        else tLength++;
    }
    if (tLength > 0) 
    {
        tokens.push_back(input.substr(sIndex, tLength));
    }
    return std::move(tokens);
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
        std::vector<std::string> tokens = tokenize(input);
        
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

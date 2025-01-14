//file scritto da Alessandro Farina

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <memory>
#include <algorithm>

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

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
        if(dm.checkList(args)) return 2;
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




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

#ifndef DEVICE_H
#define DEVICE_H

class Device
{
	private:
		std::string name;
		std::string id;
		double tdp;				//potenza
		double prt; 			//priorita'
		bool status = false;	//stato: true = acceso | false = spento




	public:
		//costruttori
		Device();

		void powerOn();
		void powerOff();



};

#endif //DEVICE_H

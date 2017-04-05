#ifndef TREASURY_HEADER
#define TREASURY_HEADER

#include <string>  //std::std::std::string
#include <sstream> //std::stringstream

struct Treasury
{
	Treasury(double Percent, double PercentChange) : 
		Percent(Percent), PercentChange(PercentChange) {};
	Treasury(std::string Date, double Percent, double PercentChange) :
		Date(Date), Percent(Percent), PercentChange(PercentChange) {};
	Treasury(std::string Date, std::string Percent, std::string PercentChange);

	std::string Date = "NULL";
	double Percent = 0;
	double PercentChange = 0;
};

Treasury::Treasury(std::string Date, std::string Percent, std::string PercentChange) : Date(Date)
{
	std::stringstream ss;

	ss << Percent; ss >> this->Percent;

	ss.clear(); ss.str("");
	ss << PercentChange; ss >> this->PercentChange;
}


#endif
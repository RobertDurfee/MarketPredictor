#ifndef CURRENCY_HEADER
#define CURRENCY_HEADER

#include <string>  //std::std::std::string
#include <sstream> //std::stringstream

struct Currency
{
	Currency(double Rate, double PercentChange) : 
		Rate(Rate), PercentChange(PercentChange) {};
	Currency(std::string Date, double Rate, double PercentChange) :
		Date(Date), Rate(Rate), PercentChange(PercentChange) {};
	Currency(std::string Date, std::string Rate, std::string PercentChange);

	std::string Date = "NULL";
	double Rate = 0;
	double PercentChange = 0;
};

Currency::Currency(std::string Date, std::string Rate, std::string PercentChange) : Date(Date)
{
	std::stringstream ss;

	ss << Rate; ss >> this->Rate;

	ss.clear(); ss.str("");
	ss << PercentChange; ss >> this->PercentChange;
}

#endif
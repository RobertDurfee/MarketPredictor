#ifndef COMMODITY_HEADER
#define COMMODITY_HEADER

#include <string>  //std::std::std::string
#include <sstream> //std::stringstream

struct Commodity
{
	Commodity(double Price, double PercentChange) : 
		Price(Price), PercentChange(PercentChange) {};
	Commodity(std::string Date, double Price, double PercentChange) :
		Date(Date), Price(Price), PercentChange(PercentChange) {};
	Commodity(std::string Date, std::string Price, std::string PercentChange);
	
	std::string Date = "NULL";
	double Price = 0;
	double PercentChange = 0;
};

Commodity::Commodity(std::string Date, std::string Price, std::string PercentChange) : Date(Date)
{
	std::stringstream ss;

	ss << Price; ss >> this->Price;

	ss.clear(); ss.str("");
	ss << PercentChange; ss >> this->PercentChange;
}

#endif
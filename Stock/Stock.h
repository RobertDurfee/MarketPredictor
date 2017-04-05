#ifndef STOCK_HEADER
#define STOCK_HEADER

#include <string>  //std::std::string
#include <sstream> //std::stringstream

struct Stock
{
	Stock(double Open, double High, double Low, double Close, double Volume, double AdjustedClose, double PercentChange) :
		Open(Open), High(High), Low(Low), Close(Close), Volume(Volume), AdjustedClose(AdjustedClose), PercentChange(PercentChange) {};
	Stock(std::string Date, double Open, double High, double Low, double Close, double Volume, double AdjustedClose, double PercentChange) :
		Date(Date), Open(Open), High(High), Low(Low), Close(Close), Volume(Volume), AdjustedClose(AdjustedClose), PercentChange(PercentChange) {};
	Stock(std::string Date, std::string Open, std::string High, std::string Low, std::string Close, std::string Volume, std::string AdjustedClose, std::string PercentChange);

	std::string Date = "NULL";
	double Open = 0;
	double High = 0;
	double Low = 0;
	double Close = 0;
	double Volume = 0;
	double AdjustedClose = 0;
	double PercentChange = 0;
};

Stock::Stock(std::string Date, std::string Open, std::string High, std::string Low, std::string Close, std::string Volume, std::string AdjustedClose, std::string PercentChange) : Date(Date)
{
	std::stringstream ss;

	ss << Open; ss >> this->Open;

	ss.clear(); ss.str("");
	ss << High; ss >> this->High;

	ss.clear(); ss.str("");
	ss << Low; ss >> this->Low;

	ss.clear(); ss.str("");
	ss << Close; ss >> this->Close;

	ss.clear(); ss.str("");
	ss << Volume; ss >> this->Volume;

	ss.clear(); ss.str("");
	ss << AdjustedClose; ss >> this->AdjustedClose;

	ss.clear(); ss.str("");
	ss << PercentChange; ss >> this->PercentChange;
}

#endif
#ifndef SCHEMAS_HEADER
#define SCHEMAS_HEADER

#include <string>  //std::string

struct QuandlSchema
{
	QuandlSchema(std::string Database, std::string Datasheet, std::string Name, std::string Datatype) :
		Database(Database), Datasheet(Datasheet), Name(Name), Datatype(Datatype) {};

	std::string Database;
	std::string Datasheet;
	std::string Name;
	std::string Datatype;
};

struct YahooSchema
{
	YahooSchema(std::string Symbol, std::string Name) : 
		Symbol(Symbol), Name(Name) {};

	std::string Symbol;
	std::string Name;
};

#endif
#ifndef PREDICTOR_HEADER
#define PREDICTOR_HEADER

#define _CRT_SECURE_NO_WARNINGS //For localtime()

#include "HTTPAlgorithms.h"   //RetrieveWebpage()
#include "ArtificialNeuralNetwork.h" //NeuralData
#include <string>             //string
#include <sstream>            //stringstream
#include <vector>             //vector
#include <armadillo>          //arma::Col
#include <mysql.h>            //MYSQL, mysql_real_connect(), mysql_close(), mysql_query(), MYSQL_RES, MYSQL_ROW, mysql_free_result(), mysql_store_result(), mysql_fetch_row()
#include <time.h>             //tm, time_t, mktime(), localtime()

#include "Stock.h"            //Stock
#include "Treasury.h"         //Treasury
#include "Commodity.h"        //Commodity
#include "Currency.h"         //Currency

#include "Schemas.h"          //YahooSchema, QuandlSchema

#include "PredictorTypes.h"   //Date, Dates, PredictionSet, PredictionActualPair, Actuals, Predictions

//Defined to clarify functions with both input and output parameters.
#define IN
#define OUT

//I know, I know: Don't use this in a header file, but I am sick of typing/seeing 'std::' everywhere.
using namespace std;

struct PredictionInformation
{
	PredictionSet Values;

	double GainWithout;
	double GainWith;
	double GainBest;
};

enum class Statistic { None, Mean, StdDev };

class Predictor
{
public:
	Predictor(NeuralDimensions sizes, string symbol, string server, string user, string password, bool initializeDatabase);
	Predictor(NeuralDimensions sizes, string symbol, bool initializeDatabase) : 
		Predictor(sizes, symbol, "<HOSTNAME>", "<USERNAME>", "<PASSWORD>", initializeDatabase) {};

	Predictor(NeuralNetwork neuralNetwork, string symbol, string server, string user, string password, bool initializeDatabase);
	Predictor(NeuralNetwork neuralNetwork, string symbol, bool initializeDatabase) :
		Predictor(neuralNetwork, symbol, "<HOSTNAME>", "<USERNAME>", "<PASSWORD>", initializeDatabase) {};

	Predictor(string neuralNetworkFilename, string symbol, string server, string user, string password, bool initializeDatabase);
	Predictor(string neuralNetworkFilename, string symbol, bool initializeDatabase) :
		Predictor(neuralNetworkFilename, symbol, "<HOSTNAME>", "<USERNAME>", "<PASSWORD>", initializeDatabase) {};

	~Predictor();

	PredictionInformation * TrainPredict(int epochs, int minibatchsize, double learningrate, string startingdate, int numberofdates, int numberofiterations);

	void Train(int epochs, int minibatchsize, double learningrate, string startingdate, int numberofdates);
	PredictionInformation * Predict(string startingdate, int numberofdates);

	void InitializeDatabase();

protected:
	MYSQL * databaseConnectionHandle;
	NeuralNetwork * neuralNetwork = nullptr;

	string symbol;

	void Connect(string server, string user, string password);
	void Disconnect();

	//Helper functions used by constructors
	void SetNeuralNetwork(NeuralDimensions sizes);
	void SetNeuralNetwork(NeuralNetwork neuralNetwork);
	void SetNeuralNetwork(string neuralNetworkFilename);

	//Helper functions used by TrainPredict(), Train(), Predict()
	void GetDates(IN string startingdate, IN int numberofdates, IN int numberofiterations, OUT Dates * from, OUT Dates * to);
	virtual NeuralData * GetData(Date from, Date to) = 0;
	void Gain(IN PredictionSet values, OUT double * with, OUT double * without, OUT double * best);	
	virtual void Evaluate(IN NeuralData test, OUT Predictions * predictions, OUT Actuals * actuals) = 0;

	//Helper functions used by Evaluate()
	double Normalize(double value, double mean, double stdDev);
	double Denormalize(double value, double mean, double stdDev);

	//Helper functions used by InitializeDatabase()
	void InitializeYahooData();
	void InitializeQuandlData();

	//Helper functions used by InitializeYahooData() and InitializeQuandlData()
	template<typename T> void InitializeSymbol(string symbol, string database = "NULL", string datasheet = "NULL");
	template<> void InitializeSymbol<Stock>(string symbol, string database, string datasheet);

	//Helper functions used by InitializeSymbol()
	template<typename T> vector<T> Symbols();
	template<> vector<YahooSchema> Symbols();
	template<> vector<QuandlSchema> Symbols();
	template<typename T> T Mean(vector<T> values);
	template<> Stock Mean(vector<Stock> values);
	template<> Treasury Mean(vector<Treasury> values);
	template<> Commodity Mean(vector<Commodity> values);
	template<> Currency Mean(vector<Currency> values);
	template<typename T> T StdDev(vector<T> values, T mean);
	template<> Stock StdDev(vector<Stock> values, Stock mean);
	template<> Treasury StdDev(vector<Treasury> values, Treasury mean);
	template<> Commodity StdDev(vector<Commodity> values, Commodity mean);
	template<> Currency StdDev(vector<Currency> values, Currency mean);

	//HTML scraping functions
	template<typename T> vector<T> Extract(string rawData);
	template<> vector<Stock> Extract<Stock>(string rawData);
	template<> vector<Treasury> Extract<Treasury>(string rawData);
	template<> vector<Commodity> Extract<Commodity>(string rawData);
	template<> vector<Currency> Extract<Currency>(string rawData);

	//SQL insert operation functions
	template<typename T> void Insert(string symbol, T value, Statistic statistic = Statistic::None);
	template<> void Insert<Stock>(string symbol, Stock value, Statistic statistic);
	template<> void Insert<Treasury>(string symbol, Treasury value, Statistic statistic);
	template<> void Insert<Commodity>(string symbol, Commodity value, Statistic statistic);
	template<> void Insert<Currency>(string symbol, Currency value, Statistic statistic);

	//SQL select operation functions
	template<typename T> vector<T> Select(string symbol, Statistic statistic = Statistic::None);
	template<typename T> vector<T> Select(string symbol, Date from, Date to);
	template<> vector<Stock> Select(string symbol, Statistic statistic);
	template<> vector<Stock> Select(string symbol, Date from, Date to);
	template<> vector<Treasury> Select(string symbol, Statistic statistic);
	template<> vector<Treasury> Select(string symbol, Date from, Date to);
	template<> vector<Commodity> Select(string symbol, Statistic statistic);
	template<> vector<Commodity> Select(string symbol, Date from, Date to);
	template<> vector<Currency> Select(string symbol, Statistic statistic);
	template<> vector<Currency> Select(string symbol, Date from, Date to);

	//SQL create operation functions
	template<typename T> void Create(string tabeName);
	template<typename T> void Create(Statistic statistic);
	template<> void Create<Stock>(string tabeName);
	template<> void Create<Stock>(Statistic statistic);
	template<> void Create<Treasury>(string tabeName);
	template<> void Create<Treasury>(Statistic statistic);
	template<> void Create<Commodity>(string tabeName);
	template<> void Create<Commodity>(Statistic statistic);
	template<> void Create<Currency>(string tabeName);
	template<> void Create<Currency>(Statistic statistic);
};

//public:
Predictor::Predictor(NeuralDimensions sizes, string symbol, string server, string user, string password, bool initializeDatabase) : symbol(symbol)
{
	SetNeuralNetwork(sizes);

	Connect(server, user, password);

	if (initializeDatabase)
		InitializeDatabase();
}

Predictor::Predictor(NeuralNetwork neuralNetwork, string symbol, string server, string user, string password, bool initializeDatabase) : symbol(symbol)
{
	SetNeuralNetwork(neuralNetwork);

	Connect(server, user, password);

	if (initializeDatabase)
		InitializeDatabase();
}

Predictor::Predictor(string neuralNetworkFilename, string symbol, string server, string user, string password, bool initializeDatabase) : symbol(symbol)
{
	SetNeuralNetwork(neuralNetworkFilename);

	Connect(server, user, password);

	if (initializeDatabase)
		InitializeDatabase();
}

Predictor::~Predictor()
{
	Disconnect();
}

PredictionInformation * Predictor::TrainPredict(int epochs, int minibatchsize, double learningrate, string startingdate, int numberofdates, int numberofiterations)
{
	PredictionInformation * output = new PredictionInformation();

	Dates from, to;

	GetDates(startingdate, numberofdates, numberofiterations + 1, &from, &to);

	for (int currentIteration = 0; currentIteration < numberofiterations; currentIteration++)
	{
		output->Values.push_back(PredictionActualPair());

		NeuralData * training, * test;

		training = GetData(from[currentIteration], to[currentIteration]);
		test = GetData(from[currentIteration + 1], to[currentIteration + 1]);
	
		neuralNetwork->StochasticGradientDescent(*training, epochs, minibatchsize, learningrate);

		Predictions predictions;
		Actuals actuals;

		Evaluate(IN *test, OUT &predictions, OUT &actuals);
		
		output->Values.back().push_back(predictions);
		output->Values.back().push_back(actuals);

		delete training;
		delete test;
	}

	Gain(IN output->Values, OUT &(output->GainWith), OUT &(output->GainWithout), OUT &(output->GainBest));
	
	return output;
}

void Predictor::Train(int epochs, int minibatchsize, double learningrate, string startingdate, int numberofdates)
{
	Dates from, to;
	GetDates(startingdate, numberofdates, 1, &from, &to);

	NeuralData * training;
	training = GetData(from[0], to[0]);

	neuralNetwork->StochasticGradientDescent(*training, epochs, minibatchsize, learningrate);

	delete training;
}
PredictionInformation * Predictor::Predict(string startingdate, int numberofdates)
{
	PredictionInformation * output = new PredictionInformation();

	Dates from, to;
	GetDates(startingdate, numberofdates, 1, &from, &to);

	output->Values.push_back(PredictionActualPair());

	NeuralData * test;
	test = GetData(from[0], to[0]);

	Predictions predictions;
	Actuals actuals;
	Evaluate(IN *test, OUT &predictions, OUT &actuals);

	output->Values.back().push_back(predictions);
	output->Values.back().push_back(actuals);

	delete test;

	Gain(IN output->Values, OUT &(output->GainWith), OUT &(output->GainWithout), OUT &(output->GainBest));

	return output;
}

void Predictor::InitializeDatabase()
{
	InitializeYahooData();

	InitializeQuandlData();
}

void Predictor::InitializeYahooData()
{
	vector<YahooSchema> symbols = Symbols<YahooSchema>();

	Create<Stock>(Statistic::Mean);
	Create<Stock>(Statistic::StdDev);

	for (unsigned int symbol = 0; symbol < symbols.size(); symbol++)
		InitializeSymbol<Stock>(symbols[symbol].Symbol);
}
void Predictor::InitializeQuandlData()
{
	vector<QuandlSchema> symbols = Symbols<QuandlSchema>();

	Create<Treasury>(Statistic::Mean);
	Create<Treasury>(Statistic::StdDev);

	Create<Commodity>(Statistic::Mean);
	Create<Commodity>(Statistic::StdDev);

	Create<Currency>(Statistic::Mean);
	Create<Currency>(Statistic::StdDev);

	for (unsigned int symbol = 0; symbol < symbols.size(); symbol++)
		if (symbols[symbol].Datatype == "Treasury")
			InitializeSymbol<Treasury>(symbols[symbol].Name, symbols[symbol].Database, symbols[symbol].Datasheet);
		else if (symbols[symbol].Datatype == "Commodity")
			InitializeSymbol<Commodity>(symbols[symbol].Name, symbols[symbol].Database, symbols[symbol].Datasheet);
		else if (symbols[symbol].Datatype == "Currency")
			InitializeSymbol<Currency>(symbols[symbol].Name, symbols[symbol].Database, symbols[symbol].Datasheet);
}

//private:
void Predictor::Connect(string server, string user, string password)
{
	databaseConnectionHandle = mysql_init(nullptr);

	mysql_real_connect(databaseConnectionHandle, server.c_str(), user.c_str(), password.c_str(), NULL, 0, NULL, 0);
}
void Predictor::Disconnect()
{
	mysql_close(databaseConnectionHandle);
}

void Predictor::SetNeuralNetwork(NeuralDimensions sizes)
{
	delete neuralNetwork;
	neuralNetwork = new NeuralNetwork(sizes, nullptr); //nullptr because 'Evaluate()' implemented outside of 'neuralNetwork'
}
void Predictor::SetNeuralNetwork(NeuralNetwork neuralNetwork)
{
	delete this->neuralNetwork;
	this->neuralNetwork = new NeuralNetwork(neuralNetwork);
}
void Predictor::SetNeuralNetwork(string neuralNetworkFilename)
{
	delete neuralNetwork;
	neuralNetwork = new NeuralNetwork(neuralNetworkFilename, nullptr); //nullptr because 'Evaluate()' implemented outside of 'neuralNetwork'
}

void Predictor::GetDates(IN string startingdate, IN int numberofdates, IN int numberofiterations, OUT Dates * from, OUT Dates * to)
{
	//Pull starting month, day, and year from starting date string
	int startingyear = stoi(startingdate.substr(0, 4).c_str()) - 1900;
	int startingmonth = stoi(startingdate.substr(5, 2).c_str()) - 1;
	int startingday = stoi(startingdate.substr(8, 2).c_str());

	//Initialize tm structure to hold numerical values of dates
	tm * dateT = new tm[numberofiterations + 1];

	//Initialize stringstreams to hold string values of dates
	stringstream * dateS = new stringstream[numberofiterations + 1];

	//Use with mktime
	time_t temptime;

	//Create string for starting date
	dateT[0] = { 0, 0, 0, startingday, startingmonth, startingyear };

	//Make turn numberical value for date into a real date
	temptime = mktime(&dateT[0]);
	dateT[0] = *localtime(&temptime);

	//Turn the real numerical date into a string
	dateS[0] << dateT[0].tm_year + 1900 << "-" << dateT[0].tm_mon + 1 << "-" << dateT[0].tm_mday;

	for (int iteration = 1; iteration <= numberofiterations; iteration++)
	{
		//Set the next from date numerical value
		dateT[iteration] = { 0, 0, 0, dateT[iteration - 1].tm_mday + numberofdates, dateT[iteration - 1].tm_mon, dateT[iteration - 1].tm_year };

		//Make turn numberical value for date into a real date
		temptime = mktime(&dateT[iteration]);
		dateT[iteration] = *localtime(&temptime);

		//Turn the real numerical date into a string
		dateS[iteration] << dateT[iteration].tm_year + 1900 << "-" << dateT[iteration].tm_mon + 1 << "-" << dateT[iteration].tm_mday;
	}

	//Convert vector of dates to 'to' and 'from' pairs
	for (int iteration = 0; iteration < numberofiterations; iteration++)
	{
		from->push_back(dateS[iteration].str());
		to->push_back(dateS[iteration + 1].str());
	}
}
void Predictor::Gain(IN PredictionSet values, OUT double * with, OUT double * without, OUT double * best)
{
	*without = values.back()[ACTUAL].back() - values.front()[ACTUAL].front();
	*best = 0;
	*with = 0;

	bool holding = false, bestHolding = false;

	for (unsigned int iteration = 0; iteration < values.size(); iteration++)
		for (unsigned int value = 0; value < values[iteration][ACTUAL].size(); value++)
		{
			//End of the iteration?
			if (value < (values[iteration][PREDICTION].size() - 1))
			{
				//Buy?
				if (values[iteration][PREDICTION][value + 1] > values[iteration][ACTUAL][value] && !holding)
				{
					*with -= values[iteration][ACTUAL][value];

					holding = true;
				}

				//Sell?
				else if (values[iteration][PREDICTION][value + 1] < values[iteration][ACTUAL][value] && holding)
				{
					*with += values[iteration][ACTUAL][value];

					holding = false;
				}

				//Buy?
				if (values[iteration][ACTUAL][value + 1] > values[iteration][ACTUAL][value] && !bestHolding)
				{
					*best -= values[iteration][ACTUAL][value];

					bestHolding = true;
				}

				//Sell?
				else if (values[iteration][ACTUAL][value + 1] < values[iteration][ACTUAL][value] && bestHolding)
				{
					*best += values[iteration][ACTUAL][value];

					bestHolding = false;
				}
			}
			else
			{
				//End of all iterations?
				if (iteration < (values.size() - 1))
				{
					//Buy?
					if (values[iteration + 1][PREDICTION].front() > values[iteration][ACTUAL][value] && !holding)
					{
						*with -= values[iteration][ACTUAL][value];

						holding = true;
					}

					//Sell?
					else if (values[iteration + 1][PREDICTION].front() < values[iteration][ACTUAL][value] && holding)
					{
						*with += values[iteration][ACTUAL][value];

						holding = false;
					}

					//Buy?
					if (values[iteration + 1][ACTUAL].front() > values[iteration][ACTUAL][value] && !bestHolding)
					{
						*best -= values[iteration][ACTUAL][value];

						bestHolding = true;
					}

					//Sell?
					else if (values[iteration + 1][ACTUAL].front() < values[iteration][ACTUAL][value] && bestHolding)
					{
						*best += values[iteration][ACTUAL][value];

						bestHolding = false;
					}
				}
			}
		}
	
	//End iterations on a sell
	if (holding)
	{
		*with += values.back()[ACTUAL].back();

		holding = false;
	}

	if (bestHolding)
	{
		*best += values.back()[ACTUAL].back();

		bestHolding = false;
	}
}

double Predictor::Normalize(double value, double mean, double stdDev)
{
	return (value - mean) / stdDev;
}
double Predictor::Denormalize(double value, double mean, double stdDev)
{
	return (value * stdDev) + mean;
}

template<typename T> void Predictor::InitializeSymbol(string symbol, string database, string datasheet)
{
	stringstream URL;
	URL << "https://www.quandl.com/api/v3/datasets/" << database << "/" << datasheet << ".csv?start_date=2016-01-01&end_date=2016-12-31";

	string rawdata = RetrieveWebpage(URL.str());

	vector<T> data = Extract<T>(rawdata);

	Create<T>(symbol);

	T mean = Mean<T>(data);
	Insert<T>(symbol, mean, Statistic::Mean);

	T stdDev = StdDev<T>(data, mean);
	Insert<T>(symbol, stdDev, Statistic::StdDev);

	for (unsigned int element = 0; element < data.size() - 1; element++)
		Insert<T>(symbol, data[element]);

	Insert<T>(symbol, data.back());
}
template<> void Predictor::InitializeSymbol<Stock>(string symbol, string database, string datasheet)
{
	stringstream URL;
	URL << "http://real-chart.finance.yahoo.com/table.csv?s=" << symbol << "&a=00&b=01&c=2016&d=11&e=31&f=2016&g=d&ignore=.csv";

	string rawdata = RetrieveWebpage(URL.str());

	vector<Stock> data = Extract<Stock>(rawdata);

	Create<Stock>(symbol);

	Stock mean = Mean<Stock>(data);
	Insert<Stock>(symbol, mean, Statistic::Mean);

	Stock stdDev = StdDev<Stock>(data, mean);
	Insert<Stock>(symbol, stdDev, Statistic::StdDev);

	for (unsigned int element = 0; element < data.size() - 1; element++)
		Insert<Stock>(symbol, data[element]);

	Insert<Stock>(symbol, data.back());
}

template<> vector<YahooSchema> Predictor::Symbols<YahooSchema>()
{
	//Read S&P 500 Stock Symbols File into Character Buffer
	ifstream SP500StockSymbolsFile("S&P500.csv", ios::binary);
	SP500StockSymbolsFile.seekg(0, SP500StockSymbolsFile.end);
	int length = (int)SP500StockSymbolsFile.tellg();
	SP500StockSymbolsFile.seekg(0, SP500StockSymbolsFile.beg);
	char * buffer = new char[length];
	SP500StockSymbolsFile.read(buffer, length);
	SP500StockSymbolsFile.close();

	//Convert Character Buffer into a Vector of Symbols
	string * SymbolsString = new string(buffer);
	delete buffer;
	int index = 0;
	vector<YahooSchema> Symbols;
	while (true)
	{
		string data[2];
		for (int i = 0; i < 1; i++)
		{
			int next;
			if ((next = (int)SymbolsString->find(",", index)) == -1)
				return Symbols;
			data[i] = SymbolsString->substr(index, next - index);
			index = next + 1;
		}
		int next;
		if ((next = (int)SymbolsString->find(",\r\n", index)) == -1)
			return Symbols;
		data[1] = SymbolsString->substr(index, next - index);
		index = next + 3;
		Symbols.push_back(YahooSchema(data[0], data[1]));
	}

	return Symbols;
}
template<> vector<QuandlSchema> Predictor::Symbols<QuandlSchema>()
{
	//Read Quandl Symbols File into Character Buffer
	ifstream QuandlSymbolsFile("QuandlSymbols.csv", ios::binary);
	QuandlSymbolsFile.seekg(0, QuandlSymbolsFile.end);
	int length = (int)QuandlSymbolsFile.tellg();
	QuandlSymbolsFile.seekg(0, QuandlSymbolsFile.beg);
	char * buffer = new char[length];
	QuandlSymbolsFile.read(buffer, length);
	QuandlSymbolsFile.close();

	//Convert Character Buffer into a Vector of Symbols
	string * SymbolsString = new string(buffer);
	delete buffer;
	int index = 0;
	index = (int)SymbolsString->find("\n", index) + (int)strlen("\n");
	vector<QuandlSchema> Symbols;
	while (true)
	{
		string data[4];
		for (int i = 0; i < 3; i++)
		{
			int next;
			if ((next = (int)SymbolsString->find(",", index)) == -1)
				return Symbols;
			data[i] = SymbolsString->substr(index, next - index);
			index = next + 1;
		}
		int next;
		if ((next = (int)SymbolsString->find("\n", index)) == -1)
			return Symbols;
		data[3] = SymbolsString->substr(index, next - index);
		index = next + 1;
		Symbols.push_back(QuandlSchema(data[0], data[1], data[2], data[3]));
	}

	return Symbols;
}

template<> Stock Predictor::Mean<Stock>(vector<Stock> values)
{
	Stock mean(0, 0, 0, 0, 0, 0, 0);

	for (unsigned int value = 0; value < values.size(); value++)
	{
		mean.Open += values[value].Open;
		mean.High += values[value].High;
		mean.Low += values[value].Low;
		mean.Close += values[value].Close;
		mean.Volume += values[value].Volume;
		mean.AdjustedClose += values[value].AdjustedClose;
		mean.PercentChange += values[value].PercentChange;
	}

	mean.Open /= double(values.size());
	mean.High /= double(values.size());
	mean.Low /= double(values.size());
	mean.Close /= double(values.size());
	mean.Volume /= double(values.size());
	mean.AdjustedClose /= double(values.size());
	mean.PercentChange /= double(values.size());

	return mean;
}
template<> Treasury Predictor::Mean<Treasury>(vector<Treasury> values)
{
	Treasury mean(0, 0);

	for (unsigned int value = 0; value < values.size(); value++)
	{
		mean.Percent += values[value].Percent;
		mean.PercentChange += values[value].PercentChange;
	}

	mean.Percent /= double(values.size());
	mean.PercentChange /= double(values.size());

	return mean;
}
template<> Commodity Predictor::Mean<Commodity>(vector<Commodity> values)
{
	Commodity mean(0, 0);

	for (unsigned int value = 0; value < values.size(); value++)
	{
		mean.Price += values[value].Price;
		mean.PercentChange += values[value].PercentChange;
	}

	mean.Price /= double(values.size());
	mean.PercentChange /= double(values.size());

	return mean;
}
template<> Currency Predictor::Mean<Currency>(vector<Currency> values)
{
	Currency mean(0, 0);

	for (unsigned int value = 0; value < values.size(); value++)
	{
		mean.Rate += values[value].Rate;
		mean.PercentChange += values[value].PercentChange;
	}

	mean.Rate /= double(values.size());
	mean.PercentChange /= double(values.size());

	return mean;
}

template<> Stock Predictor::StdDev<Stock>(vector<Stock> values, Stock mean)
{
	Stock stdDev(0, 0, 0, 0, 0, 0, 0);

	double oldVolume = 0;

	for (unsigned int value = 0; value < values.size(); value++)
	{
		stdDev.Open += pow(values[value].Open - mean.Open, 2);
		stdDev.High += pow(values[value].High - mean.High, 2);
		stdDev.Low += pow(values[value].Low - mean.Low, 2);
		stdDev.Close += pow(values[value].Close - mean.Close, 2);
		stdDev.Volume += pow(values[value].Volume - mean.Volume, 2);
		stdDev.AdjustedClose += pow(values[value].AdjustedClose - mean.AdjustedClose, 2);
		stdDev.PercentChange += pow(values[value].PercentChange - mean.PercentChange, 2);

		if (stdDev.Volume < oldVolume)
			throw; //OVERFLOW!!!
		oldVolume = stdDev.Volume;
	}

	stdDev.Open = sqrt(stdDev.Open / double(values.size()));
	stdDev.High = sqrt(stdDev.High / double(values.size()));
	stdDev.Low = sqrt(stdDev.Low / double(values.size()));
	stdDev.Close = sqrt(stdDev.Close / double(values.size()));
	stdDev.Volume = sqrt(stdDev.Volume / double(values.size()));
	stdDev.AdjustedClose = sqrt(stdDev.AdjustedClose / double(values.size()));
	stdDev.PercentChange = sqrt(stdDev.PercentChange / double(values.size()));

	return stdDev;
}
template<> Treasury Predictor::StdDev<Treasury>(vector<Treasury> values, Treasury mean)
{
	Treasury stdDev(0, 0);

	for (unsigned int value = 0; value < values.size(); value++)
	{
		stdDev.Percent += pow(values[value].Percent - mean.Percent, 2);
		stdDev.PercentChange += pow(values[value].PercentChange - mean.PercentChange, 2);
	}

	stdDev.Percent = sqrt(stdDev.Percent / double(values.size()));
	stdDev.PercentChange = sqrt(stdDev.PercentChange / double(values.size()));

	return stdDev;
}
template<> Commodity Predictor::StdDev<Commodity>(vector<Commodity> values, Commodity mean)
{
	Commodity stdDev(0, 0);

	for (unsigned int value = 0; value < values.size(); value++)
	{
		stdDev.Price += pow(values[value].Price - mean.Price, 2);
		stdDev.PercentChange += pow(values[value].PercentChange - mean.PercentChange, 2);
	}

	stdDev.Price = sqrt(stdDev.Price / double(values.size()));
	stdDev.PercentChange = sqrt(stdDev.PercentChange / double(values.size()));

	return stdDev;
}
template<> Currency Predictor::StdDev<Currency>(vector<Currency> values, Currency mean)
{
	Currency stdDev(0, 0);

	for (unsigned int value = 0; value < values.size(); value++)
	{
		stdDev.Rate += pow(values[value].Rate - mean.Rate, 2);
		stdDev.PercentChange += pow(values[value].PercentChange - mean.PercentChange, 2);
	}

	stdDev.Rate = sqrt(stdDev.Rate / double(values.size()));
	stdDev.PercentChange = sqrt(stdDev.PercentChange / double(values.size()));

	return stdDev;
}

template<> vector<Stock> Predictor::Extract<Stock>(string rawData)
{
	vector<Stock> output;

	int index = 0;

	index = (int)rawData.find(",", index);
	index = (int)rawData.find("\n", index) + (int)strlen("\n");

	while (true)
	{
		string data[7];
		for (int i = 0; i < 6; i++)
		{
			int next;
			if ((next = (int)rawData.find(",", index)) == -1)
				goto BREAKALL;
			data[i] = rawData.substr(index, next - index);
			index = next + 1;
		}

		int next;
		if ((next = (int)rawData.find("\n", index)) == -1)
			goto BREAKALL;
		data[6] = rawData.substr(index, next - index);
		index = next + 1;

		output.push_back(Stock(data[0], data[1], data[2], data[3], data[4], data[5], data[6], "0"));
	}

BREAKALL:

	for (unsigned int value = 0; value < output.size() - 1; value++)
		output[value].PercentChange = (output[value].AdjustedClose - output[value + 1].AdjustedClose) / output[value + 1].AdjustedClose;

	return output;
}
template<> vector<Treasury> Predictor::Extract<Treasury>(string rawData)
{
	vector<Treasury> output;

	int index = 0;

	index = (int)rawData.find(",", index);
	index = (int)rawData.find("\n", index) + (int)strlen("\n");

	while (true)
	{
		string data[2];
		for (int i = 0; i < 1; i++)
		{
			int next;
			if ((next = (int)rawData.find(",", index)) == -1)
				goto BREAKALL;
			data[i] = rawData.substr(index, next - index);
			index = next + 1;
		}

		int next;
		if ((next = (int)rawData.find("\n", index)) == -1)
			goto BREAKALL;
		data[1] = rawData.substr(index, next - index);
		index = next + 1;

		output.push_back(Treasury(data[0], data[1], "0"));
	}

BREAKALL:

	for (unsigned int value = 0; value < output.size() - 1; value++)
		output[value].PercentChange = (output[value].Percent - output[value + 1].Percent) / output[value + 1].Percent;

	return output;
}
template<> vector<Commodity> Predictor::Extract<Commodity>(string rawData)
{
	vector<Commodity> output;

	int index = 0;

	index = (int)rawData.find(",", index);
	index = (int)rawData.find("\n", index) + (int)strlen("\n");

	while (true)
	{
		string data[2];
		for (int i = 0; i < 1; i++)
		{
			int next;
			if ((next = (int)rawData.find(",", index)) == -1)
				goto BREAKALL;
			data[i] = rawData.substr(index, next - index);
			index = next + 1;
		}

		int next;
		if ((next = (int)rawData.find("\n", index)) == -1)
			goto BREAKALL;
		data[1] = rawData.substr(index, next - index);
		index = next + 1;

		output.push_back(Commodity(data[0], data[1], "0"));
	}

BREAKALL:

	for (unsigned int value = 0; value < output.size() - 1; value++)
		output[value].PercentChange = (output[value].Price - output[value + 1].Price) / output[value + 1].Price;

	return output;
}
template<> vector<Currency> Predictor::Extract<Currency>(string rawData)
{
	vector<Currency> output;

	int index = 0;

	index = (int)rawData.find(",", index);
	index = (int)rawData.find("\n", index) + (int)strlen("\n");

	while (true)
	{
		string data[2];
		for (int i = 0; i < 1; i++)
		{
			int next;
			if ((next = (int)rawData.find(",", index)) == -1)
				goto BREAKALL;
			data[i] = rawData.substr(index, next - index);
			index = next + 1;
		}

		int next;
		if ((next = (int)rawData.find("\n", index)) == -1)
			goto BREAKALL;
		data[1] = rawData.substr(index, next - index);
		index = next + 1;

		output.push_back(Currency(data[0], data[1], "0"));
	}

BREAKALL:

	for (unsigned int value = 0; value < output.size() - 1; value++)
		output[value].PercentChange = (output[value].Rate - output[value + 1].Rate) / output[value + 1].Rate;

	return output;
}

template<> void Predictor::Insert<Stock>(string symbol, Stock value, Statistic statistic)
{
	stringstream tableName;
	tableName << "`";
	if (statistic == Statistic::Mean)
		tableName << "Stock Mean";
	else if (statistic == Statistic::StdDev)
		tableName << "Stock StdDev";
	else
		tableName << symbol;
	tableName << "`";

	string columns;
	if (statistic == Statistic::Mean || statistic == Statistic::StdDev)
		columns = "(`Symbol`, `Open`, `High`, `Low`, `Close`, `Volume`, `AdjustedClose`, `PercentChange`)";
	else
		columns = "(`Date`, `Open`, `High`, `Low`, `Close`, `Volume`, `AdjustedClose`, `PercentChange`)";

	stringstream values;
	values << "(";
	if (statistic == Statistic::Mean || statistic == Statistic::StdDev)
		values << "'" << symbol << "', " << value.Open << ", " << value.High << ", " << value.Low << ", " << value.Close << ", " << value.Volume << ", " << value.AdjustedClose << ", " << value.PercentChange;
	else
		values << "'" << value.Date << "', " << value.Open << ", " << value.High << ", " << value.Low << ", " << value.Close << ", " << value.Volume << ", " << value.AdjustedClose << ", " << value.PercentChange;
	values << ")";

	stringstream query;
	query << "INSERT INTO `EconomicData`." << tableName.str() << " " << columns << " VALUES " << values.str();

	mysql_query(databaseConnectionHandle, query.str().c_str());
}
template<> void Predictor::Insert<Treasury>(string symbol, Treasury value, Statistic statistic)
{
	stringstream tableName;
	tableName << "`";
	if (statistic == Statistic::Mean)
		tableName << "Treasury Mean";
	else if (statistic == Statistic::StdDev)
		tableName << "Treasury StdDev";
	else
		tableName << symbol;
	tableName << "`";

	string columns;
	if (statistic == Statistic::Mean || statistic == Statistic::StdDev)
		columns = "(`Symbol`, `Percent`, `PercentChange`)";
	else
		columns = "(`Date`, `Percent`, `PercentChange`)";

	stringstream values;
	values << "(";
	if (statistic == Statistic::Mean || statistic == Statistic::StdDev)
		values << "'" << symbol << "', " << value.Percent << ", " << value.PercentChange;
	else
		values << "'" << value.Date << "', " << value.Percent << ", " << value.PercentChange;
	values << ")";

	stringstream query;
	query << "INSERT INTO `EconomicData`." << tableName.str() << " " << columns << " VALUES " << values.str();

	mysql_query(databaseConnectionHandle, query.str().c_str());
}
template<> void Predictor::Insert<Commodity>(string symbol, Commodity value, Statistic statistic)
{
	stringstream tableName;
	tableName << "`";
	if (statistic == Statistic::Mean)
		tableName << "Commodity Mean";
	else if (statistic == Statistic::StdDev)
		tableName << "Commodity StdDev";
	else
		tableName << symbol;
	tableName << "`";

	string columns;
	if (statistic == Statistic::Mean || statistic == Statistic::StdDev)
		columns = "(`Symbol`, `Price`, `PercentChange`)";
	else
		columns = "(`Date`, `Price`, `PercentChange`)";

	stringstream values;
	values << "(";
	if (statistic == Statistic::Mean || statistic == Statistic::StdDev)
		values << "'" << symbol << "', " << value.Price << ", " << value.PercentChange;
	else
		values << "'" << value.Date << "', " << value.Price << ", " << value.PercentChange;
	values << ")";

	stringstream query;
	query << "INSERT INTO `EconomicData`." << tableName.str() << " " << columns << " VALUES " << values.str();

	mysql_query(databaseConnectionHandle, query.str().c_str());
}
template<> void Predictor::Insert<Currency>(string symbol, Currency value, Statistic statistic)
{
	stringstream tableName;
	tableName << "`";
	if (statistic == Statistic::Mean)
		tableName << "Currency Mean";
	else if (statistic == Statistic::StdDev)
		tableName << "Currency StdDev";
	else
		tableName << symbol;
	tableName << "`";

	string columns;
	if (statistic == Statistic::Mean || statistic == Statistic::StdDev)
		columns = "(`Symbol`, `Rate`, `PercentChange`)";
	else
		columns = "(`Date`, `Rate`, `PercentChange`)";

	stringstream values;
	values << "(";
	if (statistic == Statistic::Mean || statistic == Statistic::StdDev)
		values << "'" << symbol << "', " << value.Rate << ", " << value.PercentChange;
	else
		values << "'" << value.Date << "', " << value.Rate << ", " << value.PercentChange;
	values << ")";

	stringstream query;
	query << "INSERT INTO `EconomicData`." << tableName.str() << " " << columns << " VALUES " << values.str();

	mysql_query(databaseConnectionHandle, query.str().c_str());
}

template<> vector<Stock> Predictor::Select<Stock>(string symbol, Statistic statistic)
{
	string columns;
	if (statistic == Statistic::Mean || statistic == Statistic::StdDev)
		columns = "`Symbol`, `Open`, `High`, `Low`, `Close`, `Volume`, `AdjustedClose`, `PercentChange`";
	else
		columns = "`Date`, `Open`, `High`, `Low`, `Close`, `Volume`, `AdjustedClose`, `PercentChange`";

	stringstream tableName;
	tableName << "`";
	if (statistic == Statistic::Mean)
		tableName << "Stock Mean";
	else if (statistic == Statistic::StdDev)
		tableName << "Stock StdDev";
	else
		tableName << symbol;
	tableName << "`";

	stringstream where_;
	if (statistic == Statistic::Mean || statistic == Statistic::StdDev)
		where_ << "WHERE `Symbol` = '" << symbol << "'";

	stringstream query;
	query << "SELECT " << columns << " FROM `EconomicData`." << tableName.str() << " " << where_.str();

	vector<Stock> output;

	mysql_query(databaseConnectionHandle, query.str().c_str());

	MYSQL_RES * result = mysql_store_result(databaseConnectionHandle);

	MYSQL_ROW row;

	while (row = mysql_fetch_row(result))
		output.push_back(Stock(row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7]));

	mysql_free_result(result);

	return output;
}
template<> vector<Stock> Predictor::Select<Stock>(string symbol, Date from, Date to)
{
	string columns = "`Date`, `Open`, `High`, `Low`, `Close`, `Volume`, `AdjustedClose`, `PercentChange`";

	stringstream tableName;
	tableName << "`" << symbol << "`";

	stringstream where_;
	where_ << "WHERE `Date` >= STR_TO_DATE('" << from << "', '%Y-%m-%d') AND `Date` <= STR_TO_DATE('" << to << "', '%Y-%m-%d')";

	stringstream query;
	query << "SELECT " << columns << " FROM `EconomicData`." << tableName.str() << " " << where_.str();

	vector<Stock> output;

	mysql_query(databaseConnectionHandle, query.str().c_str());

	MYSQL_RES * result = mysql_store_result(databaseConnectionHandle);

	MYSQL_ROW row;

	while (row = mysql_fetch_row(result))
		output.push_back(Stock(row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7]));

	mysql_free_result(result);

	return output;
}
template<> vector<Treasury> Predictor::Select<Treasury>(string symbol, Statistic statistic)
{
	string columns;
	if (statistic == Statistic::Mean || statistic == Statistic::StdDev)
		columns = "`Symbol`, `Percent`, `PercentChange`";
	else
		columns = "`Date`, `Percent`, `PercentChange`";

	stringstream tableName;
	tableName << "`";
	if (statistic == Statistic::Mean)
		tableName << "Treasury Mean";
	else if (statistic == Statistic::StdDev)
		tableName << "Treasury StdDev";
	else
		tableName << symbol;
	tableName << "`";

	stringstream where_;
	if (statistic == Statistic::Mean || statistic == Statistic::StdDev)
		where_ << "WHERE `Symbol` = '" << symbol << "'";

	stringstream query;
	query << "SELECT " << columns << " FROM `EconomicData`." << tableName.str() << " " << where_.str();

	vector<Treasury> output;

	mysql_query(databaseConnectionHandle, query.str().c_str());

	MYSQL_RES * result = mysql_store_result(databaseConnectionHandle);

	MYSQL_ROW row;

	while (row = mysql_fetch_row(result))
		output.push_back(Treasury(row[0], row[1], row[2]));

	mysql_free_result(result);

	return output;
}
template<> vector<Treasury> Predictor::Select<Treasury>(string symbol, Date from, Date to)
{
	string columns = "`Date`, `Percent`, `PercentChange`";

	stringstream tableName;
	tableName << "`" << symbol << "`";

	stringstream where_;
	where_ << "WHERE `Date` >= STR_TO_DATE('" << from << "', '%Y-%m-%d') AND `Date` < STR_TO_DATE('" << to << "', '%Y-%m-%d')";

	stringstream query;
	query << "SELECT " << columns << " FROM `EconomicData`." << tableName.str() << " " << where_.str();

	vector<Treasury> output;

	mysql_query(databaseConnectionHandle, query.str().c_str());

	MYSQL_RES * result = mysql_store_result(databaseConnectionHandle);

	MYSQL_ROW row;

	while (row = mysql_fetch_row(result))
		output.push_back(Treasury(row[0], row[1], row[2]));

	mysql_free_result(result);

	return output;
}
template<> vector<Commodity> Predictor::Select<Commodity>(string symbol, Statistic statistic)
{
	string columns;
	if (statistic == Statistic::Mean || statistic == Statistic::StdDev)
		columns = "`Symbol`, `Price`, `PercentChange`";
	else
		columns = "`Date`, `Price`, `PercentChange`";

	stringstream tableName;
	tableName << "`";
	if (statistic == Statistic::Mean)
		tableName << "Commodity Mean";
	else if (statistic == Statistic::StdDev)
		tableName << "Commodity StdDev";
	else
		tableName << symbol;
	tableName << "`";

	stringstream where_;
	if (statistic == Statistic::Mean || statistic == Statistic::StdDev)
		where_ << "WHERE `Symbol` = '" << symbol << "'";

	stringstream query;
	query << "SELECT " << columns << " FROM `EconomicData`." << tableName.str() << " " << where_.str();

	vector<Commodity> output;

	mysql_query(databaseConnectionHandle, query.str().c_str());

	MYSQL_RES * result = mysql_store_result(databaseConnectionHandle);

	MYSQL_ROW row;

	while (row = mysql_fetch_row(result))
		output.push_back(Commodity(row[0], row[1], row[2]));

	mysql_free_result(result);

	return output;
}
template<> vector<Commodity> Predictor::Select<Commodity>(string symbol, Date from, Date to)
{
	string columns = "`Date`, `Price`, `PercentChange`";

	stringstream tableName;
	tableName << "`" << symbol << "`";

	stringstream where_;
	where_ << "WHERE `Date` >= STR_TO_DATE('" << from << "', '%Y-%m-%d') AND `Date` < STR_TO_DATE('" << to << "', '%Y-%m-%d')";

	stringstream query;
	query << "SELECT " << columns << " FROM `EconomicData`." << tableName.str() << " " << where_.str();

	vector<Commodity> output;

	mysql_query(databaseConnectionHandle, query.str().c_str());

	MYSQL_RES * result = mysql_store_result(databaseConnectionHandle);

	MYSQL_ROW row;

	while (row = mysql_fetch_row(result))
		output.push_back(Commodity(row[0], row[1], row[2]));

	mysql_free_result(result);

	return output;
}
template<> vector<Currency> Predictor::Select<Currency>(string symbol, Statistic statistic)
{
	string columns;
	if (statistic == Statistic::Mean || statistic == Statistic::StdDev)
		columns = "`Symbol`, `Rate`, `PercentChange`";
	else
		columns = "`Date`, `Rate`, `PercentChange`";

	stringstream tableName;
	tableName << "`";
	if (statistic == Statistic::Mean)
		tableName << "Currency Mean";
	else if (statistic == Statistic::StdDev)
		tableName << "Currency StdDev";
	else
		tableName << symbol;
	tableName << "`";

	stringstream where_;
	if (statistic == Statistic::Mean || statistic == Statistic::StdDev)
		where_ << "WHERE `Symbol` = '" << symbol << "'";

	stringstream query;
	query << "SELECT " << columns << " FROM `EconomicData`." << tableName.str() << " " << where_.str();

	vector<Currency> output;

	mysql_query(databaseConnectionHandle, query.str().c_str());

	MYSQL_RES * result = mysql_store_result(databaseConnectionHandle);

	MYSQL_ROW row;

	while (row = mysql_fetch_row(result))
		output.push_back(Currency(row[0], row[1], row[2]));

	mysql_free_result(result);

	return output;
}
template<> vector<Currency> Predictor::Select<Currency>(string symbol, Date from, Date to)
{
	string columns = "`Date`, `Price`, `PercentChange`";

	stringstream tableName;
	tableName << "`" << symbol << "`";

	stringstream where_;
	where_ << "WHERE `Date` >= STR_TO_DATE('" << from << "', '%Y-%m-%d') AND `Date` < STR_TO_DATE('" << to << "', '%Y-%m-%d')";

	stringstream query;
	query << "SELECT " << columns << " FROM `EconomicData`." << tableName.str() << " " << where_.str();

	vector<Currency> output;

	mysql_query(databaseConnectionHandle, query.str().c_str());

	MYSQL_RES * result = mysql_store_result(databaseConnectionHandle);

	MYSQL_ROW row;

	while (row = mysql_fetch_row(result))
		output.push_back(Currency(row[0], row[1], row[2]));

	mysql_free_result(result);

	return output;
}

template<> void Predictor::Create<Stock>(string tableName)
{
	stringstream tablename;
	tablename << "`" << tableName << "`";

	string columns = "( `ID` int NOT NULL AUTO_INCREMENT, \
						`Date` DATE NOT NULL,             \
						`Open` FLOAT NOT NULL,            \
						`High` FLOAT NOT NULL,            \
						`Low` FLOAT NOT NULL,             \
						`Close` FLOAT NOT NULL,           \
						`Volume` FLOAT NOT NULL,          \
						`AdjustedClose` FLOAT NOT NULL,   \
						`PercentChange` FLOAT NOT NULL,   \
					PRIMARY KEY(`ID`))";

	stringstream query;
	query << "CREATE TABLE `EconomicData`." << tablename.str() << " " << columns;

	mysql_query(databaseConnectionHandle, query.str().c_str());
}
template<> void Predictor::Create<Stock>(Statistic statistic)
{
	stringstream tableName;
	tableName << "`";
	if (statistic == Statistic::Mean)
		tableName << "Stock Mean";
	else if (statistic == Statistic::StdDev)
		tableName << "Stock StdDev";
	tableName << "`";

	string columns = "( `ID` int NOT NULL AUTO_INCREMENT, \
						`Symbol` VARCHAR(255) NOT NULL,   \
						`Open` FLOAT NOT NULL,            \
						`High` FLOAT NOT NULL,            \
						`Low` FLOAT NOT NULL,             \
						`Close` FLOAT NOT NULL,           \
						`Volume` FLOAT NOT NULL,          \
						`AdjustedClose` FLOAT NOT NULL,   \
						`PercentChange` FLOAT NOT NULL,   \
					PRIMARY KEY(`ID`))";

	stringstream query;
	query << "CREATE TABLE `EconomicData`." << tableName.str() << " " << columns;

	mysql_query(databaseConnectionHandle, query.str().c_str());
}
template<> void Predictor::Create<Treasury>(string tableName)
{
	stringstream tablename;
	tablename << "`" << tableName << "`";

	string columns = "( `ID` int NOT NULL AUTO_INCREMENT, \
						`Date` date NOT NULL,             \
						`Percent` float NOT NULL,         \
						`PercentChange` float NOT NULL,   \
					PRIMARY KEY(`ID`))";

	stringstream query;
	query << "CREATE TABLE `EconomicData`." << tablename.str() << " " << columns;

	mysql_query(databaseConnectionHandle, query.str().c_str());
}
template<> void Predictor::Create<Treasury>(Statistic statistic)
{
	stringstream tableName;
	tableName << "`";
	if (statistic == Statistic::Mean)
		tableName << "Treasury Mean";
	else if (statistic == Statistic::StdDev)
		tableName << "Treasury StdDev";
	tableName << "`";

	string columns = "( `ID` int NOT NULL AUTO_INCREMENT, \
						`Symbol` varchar(255) NOT NULL,   \
						`Percent` float NOT NULL,         \
						`PercentChange` float NOT NULL,   \
					PRIMARY KEY(`ID`))";

	stringstream query;
	query << "CREATE TABLE `EconomicData`." << tableName.str() << " " << columns;

	mysql_query(databaseConnectionHandle, query.str().c_str());
}
template<> void Predictor::Create<Commodity>(string tableName)
{
	stringstream tablename;
	tablename << "`" << tableName << "`";

	string columns = "( `ID` int NOT NULL AUTO_INCREMENT, \
						`Date` date NOT NULL,             \
						`Price` float NOT NULL,           \
						`PercentChange` float NOT NULL,   \
					PRIMARY KEY(`ID`))";

	stringstream query;
	query << "CREATE TABLE `EconomicData`." << tablename.str() << " " << columns;

	mysql_query(databaseConnectionHandle, query.str().c_str());
}
template<> void Predictor::Create<Commodity>(Statistic statistic)
{
	stringstream tableName;
	tableName << "`";
	if (statistic == Statistic::Mean)
		tableName << "Commodity Mean";
	else if (statistic == Statistic::StdDev)
		tableName << "Commodity StdDev";
	tableName << "`";

	string columns = "( `ID` int NOT NULL AUTO_INCREMENT, \
						`Symbol` varchar(255) NOT NULL,   \
						`Price` float NOT NULL,           \
						`PercentChange` float NOT NULL,   \
					PRIMARY KEY(`ID`))";

	stringstream query;
	query << "CREATE TABLE `EconomicData`." << tableName.str() << " " << columns;

	mysql_query(databaseConnectionHandle, query.str().c_str());
}
template<> void Predictor::Create<Currency>(string tableName)
{
	stringstream tablename;
	tablename << "`" << tableName << "`";

	string columns = "( `ID` int NOT NULL AUTO_INCREMENT, \
						`Date` date NOT NULL,             \
						`Rate` float NOT NULL,            \
						`PercentChange` float NOT NULL,   \
					PRIMARY KEY(`ID`))";

	stringstream query;
	query << "CREATE TABLE `EconomicData`." << tablename.str() << " " << columns;

	mysql_query(databaseConnectionHandle, query.str().c_str());
}
template<> void Predictor::Create<Currency>(Statistic statistic)
{
	stringstream tableName;
	tableName << "`";
	if (statistic == Statistic::Mean)
		tableName << "Currency Mean";
	else if (statistic == Statistic::StdDev)
		tableName << "Currency StdDev";
	tableName << "`";

	string columns = "( `ID` int NOT NULL AUTO_INCREMENT, \
						`Symbol` varchar(255) NOT NULL,   \
						`Rate` float NOT NULL,            \
						`PercentChange` float NOT NULL,   \
					PRIMARY KEY(`ID`))";

	stringstream query;
	query << "CREATE TABLE `EconomicData`." << tableName.str() << " " << columns;

	mysql_query(databaseConnectionHandle, query.str().c_str());
}

#endif
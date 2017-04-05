#ifndef CURRENCY_PREDICTOR_HEADER
#define CURRENCY_PREDICTOR_HEADER

#include "Predictor.h"

class CurrencyPredictor : public Predictor
{
public:
	CurrencyPredictor(NeuralDimensions sizes, string symbol, string server, string user, string password, bool initializeDatabase = false) : 
		Predictor(sizes, symbol, server, user, password, initializeDatabase) {};
	CurrencyPredictor(NeuralNetwork neuralNetwork, string symbol, string server, string user, string password, bool initializeDatabase = false) : 
		Predictor(neuralNetwork, symbol, server, user, password, initializeDatabase) {};
	CurrencyPredictor(string neuralNetworkFilename, string symbol, string server, string user, string password, bool initializeDatabase = false) : 
		Predictor(neuralNetworkFilename, symbol, server, user, password, initializeDatabase) {};
	
protected:
	NeuralData * GetData(Date from, Date to);
	void Evaluate(IN NeuralData test, OUT vector<double> * predictions, OUT vector<double> * actuals);
};

NeuralData * CurrencyPredictor::GetData(Date from, Date to)
{
	NeuralData * output = new NeuralData();

	vector<Currency> data = Select<Currency>(symbol, from, to);
	vector<Currency> mean = Select<Currency>(symbol, Statistic::Mean);
	vector<Currency> stdDev = Select<Currency>(symbol, Statistic::StdDev);

	for (int i = (int)data.size() - 1; i >= 0; i--) //Check this!! Robert, you should order the select query!
	{
		output->push_back(NeuralDataPoint());

		vector<double> temp1;

		temp1.push_back(Normalize(data[i].Rate, mean[0].Rate, stdDev[0].Rate));

		output->back().push_back(NeuralInput(temp1));

		vector<double> temp2;

		temp2.push_back(Normalize(data[i].PercentChange, -1.0, 2.0));

		output->back().push_back(NeuralOutput(temp2));
	}

	return output;
}

//Defined to clarify indexes
#define INPUTS            0
#define RATE           0, 0
#define PERCENT_CHANGE 0, 0

void CurrencyPredictor::Evaluate(IN NeuralData data, OUT vector<double> * predictions, OUT vector<double> * actuals)
{
	vector<Currency> mean = Select<Currency>(symbol, Statistic::Mean);
	vector<Currency> stdDev = Select<Currency>(symbol, Statistic::StdDev);

	for (int example = 0; example < data.size() - 1; example++)
	{
		NeuralOutput prediction = neuralNetwork->FeedForward(data[example][0]);

		double actualCloseToday = Denormalize(data[example][INPUTS](RATE), mean[0].Rate, stdDev[0].Rate);
		double actualCloseTomorrow = Denormalize(data[example + 1][INPUTS](RATE), mean[0].Rate, stdDev[0].Rate);

		double predictedPercentChange = Denormalize(prediction(PERCENT_CHANGE), -1, 2);

		double predictedCloseTomorrow = actualCloseToday * predictedPercentChange + actualCloseToday;

		predictions->push_back(predictedCloseTomorrow);
		actuals->push_back(actualCloseTomorrow);
	}
}

#endif
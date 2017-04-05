#ifndef STOCK_PREDICTOR_HEADER
#define STOCK_PREDICTOR_HEADER

#include "../Predictor.h"

class StockPredictor : public Predictor
{
public:
	StockPredictor(NeuralDimensions sizes, string symbol, string server, string user, string password, bool initializeDatabase = false) : 
		Predictor(sizes, symbol, server, user, password, initializeDatabase) {};
	StockPredictor(NeuralNetwork neuralNetwork, string symbol, string server, string user, string password, bool initializeDatabase = false) : 
		Predictor(neuralNetwork, symbol, server, user, password, initializeDatabase) {};
	StockPredictor(string neuralNetworkFilename, string symbol, string server, string user, string password, bool initializeDatabase = false) : 
		Predictor(neuralNetworkFilename, symbol, server, user, password, initializeDatabase) {};
		
private:
	NeuralData * GetData(Date from, Date to);
	void Evaluate(IN NeuralData test, OUT vector<double> * predictions, OUT vector<double> * actuals);
};

NeuralData * StockPredictor::GetData(Date from, Date to)
{
	NeuralData * output = new NeuralData();

	vector<Stock> data = Select<Stock>(symbol, from, to);
	vector<Stock> mean = Select<Stock>(symbol, Statistic::Mean);
	vector<Stock> stdDev = Select<Stock>(symbol, Statistic::StdDev);

	for (int i = (int)data.size() - 1; i >= 0; i--) //Check this!! Robert, you should order the select query!
	{
		output->push_back(NeuralDataPoint());

		vector<double> temp1;

		temp1.push_back(Normalize(data[i].Open, mean[0].Open, stdDev[0].Open));
		temp1.push_back(Normalize(data[i].High, mean[0].High, stdDev[0].High));
		temp1.push_back(Normalize(data[i].Low, mean[0].Low, stdDev[0].Low));
		temp1.push_back(Normalize(data[i].AdjustedClose, mean[0].AdjustedClose, stdDev[0].AdjustedClose));
		temp1.push_back(Normalize(data[i].Volume, mean[0].Volume, stdDev[0].Volume));

		output->back().push_back(NeuralInput(temp1));

		vector<double> temp2;

		temp2.push_back(Normalize(data[i].PercentChange, -1.0, 2.0));

		output->back().push_back(NeuralOutput(temp2));
	}

	return output;
}

//Defined to clarify indexes
#define INPUTS            0
#define CLOSE          3, 0
#define PERCENT_CHANGE 0, 0

void StockPredictor::Evaluate(IN NeuralData data, OUT vector<double> * predictions, OUT vector<double> * actuals)
{
	vector<Stock> mean = Select<Stock>(symbol, Statistic::Mean);
	vector<Stock> stdDev = Select<Stock>(symbol, Statistic::StdDev);

	for (int example = 0; example < data.size() - 1; example++) //Do I really want to cut off the last one??
	{
		NeuralOutput prediction = neuralNetwork->FeedForward(data[example][0]);

		double actualCloseToday = Denormalize(data[example][INPUTS](CLOSE), mean[0].Close, stdDev[0].Close);
		double actualCloseTomorrow = Denormalize(data[example + 1][INPUTS](CLOSE), mean[0].Close, stdDev[0].Close);

		double predictedPercentChange = Denormalize(prediction(PERCENT_CHANGE), -1, 2);

		double predictedCloseTomorrow = actualCloseToday * predictedPercentChange + actualCloseToday;

		predictions->push_back(predictedCloseTomorrow);
		actuals->push_back(actualCloseTomorrow);
	}
}

#endif
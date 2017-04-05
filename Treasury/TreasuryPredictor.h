#ifndef TREASURY_PREDICTOR_HEADER
#define TREASURY_PREDICTOR_HEADER

#include "../Predictor.h"

class TreasuryPredictor : public Predictor
{
public:
	TreasuryPredictor(NeuralDimensions sizes, string symbol, string server, string user, string password, bool initializeDatabase = false) : 
		Predictor(sizes, symbol, server, user, password, initializeDatabase) {};
	TreasuryPredictor(NeuralNetwork neuralNetwork, string symbol, string server, string user, string password, bool initializeDatabase = false) : 
		Predictor(neuralNetwork, symbol, server, user, password, initializeDatabase) {};
	TreasuryPredictor(string neuralNetworkFilename, string symbol, string server, string user, string password, bool initializeDatabase = false) : 
		Predictor(neuralNetworkFilename, symbol, server, user, password, initializeDatabase) {};
	
protected:
	NeuralData * GetData(Date from, Date to);
	void Evaluate(IN NeuralData test, OUT vector<double> * predictions, OUT vector<double> * actuals);
};

NeuralData * TreasuryPredictor::GetData(Date from, Date to)
{
	NeuralData * output = new NeuralData();

	vector<Treasury> data = Select<Treasury>(symbol, from, to);
	vector<Treasury> mean = Select<Treasury>(symbol, Statistic::Mean);
	vector<Treasury> stdDev = Select<Treasury>(symbol, Statistic::StdDev);

	for (int i = (int)data.size() - 1; i >= 0; i--) //Check this!! Robert, you should order the select query!
	{
		output->push_back(NeuralDataPoint());

		vector<double> temp1;

		temp1.push_back(Normalize(data[i].Percent, mean[0].Percent, stdDev[0].Percent));

		output->back().push_back(NeuralInput(temp1));

		vector<double> temp2;

		temp2.push_back(Normalize(data[i].PercentChange, -1.0, 2.0));

		output->back().push_back(NeuralOutput(temp2));
	}

	return output;
}

//Defined to clarify indexes
#define INPUTS            0
#define PERCENT        0, 0
#define PERCENT_CHANGE 0, 0

void TreasuryPredictor::Evaluate(IN NeuralData data, OUT vector<double> * predictions, OUT vector<double> * actuals)
{
	vector<Treasury> mean = Select<Treasury>(symbol, Statistic::Mean);
	vector<Treasury> stdDev = Select<Treasury>(symbol, Statistic::StdDev);

	for (int example = 0; example < data.size() - 1; example++)
	{
		NeuralOutput prediction = neuralNetwork->FeedForward(data[example][0]);

		double actualCloseToday = Denormalize(data[example][INPUTS](PERCENT), mean[0].Percent, stdDev[0].Percent);
		double actualCloseTomorrow = Denormalize(data[example + 1][INPUTS](PERCENT), mean[0].Percent, stdDev[0].Percent);

		double predictedPercentChange = Denormalize(prediction(PERCENT_CHANGE), -1, 2);

		double predictedCloseTomorrow = actualCloseToday * predictedPercentChange + actualCloseToday;

		predictions->push_back(predictedCloseTomorrow);
		actuals->push_back(actualCloseTomorrow);
	}
}

#endif
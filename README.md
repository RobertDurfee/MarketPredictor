# MarketPredictor
C++ classes using feed-forward neural networks to predict stocks, treasuries, commodities, and currencies.

### Disclaimer
This is not production-worthy code! View this simply as a proof-of-concept. Preconditions are implicit. No error checking exists.

Additionally, this project has yet to produce statistically significant results. I still have not found the parameters and inputs necessary to make an accurate prediction. This may not even be possible as theory suggests that markets can never be predicted accurately through pattern recognition as it is chaotic in nature.

### Initialization
```C++
Predictor(NeuralDimensions sizes, string symbol, string server, string user, string password, bool initializeDatabase = false);
Predictor(NeuralNetwork neuralNetwork, string symbol, string server, string user, string password, bool initializeDatabase = false);
Predictor(string neuralNetworkFilename, string symbol, string server, string user, string password, bool initializeDatabase = false);
```
The `Predictor` interface (or abstract class) must be initialized with 

 1. The name of the stock, treasury, commodity, or currency you wish to predict.
 2. The hostname of the SQL server which stores your `EconomicData`.
 3. The username for access to the SQL server.
 4. The password for access to the SQL server.

Additionally, the `NeuralNetwork` must be initialized with either a set of `NeuralDimensions`, a `NeuralNetwork` object, or a filename to a saved `NeuralNetwork`.

### Initialize Database
```C++
void InitializeDatabase();
```
This method sets up the MySQL database for use by this class by retrieving every day's open, high, low, close, and volume for each security in the S&P 500 and major treasuries, currencies, and commodities for the year of 2016. This method takes a long time to run and progress is not indicated through the standard output. This method does not check if there is an `EconomicData` schema on the supplied MySQL connection, therefore, make sure it exists before running this method. This method only needs to be run once to utilize the class.

### Typedefs
```C++
typedef vector<double> Predictions;
typedef vector<double> Actuals;
typedef vector<Predictions /* Acutals */> PredictionActualPair;
typedef vector<PredictionActualPair> PredictionSet;
```
This definitions help understand the information contained in the `PredictionInformation` structure below.

### Prediction Information
```C++
struct PredictionInformation
{
	PredictionSet Values;

	double GainWithout;
	double GainWith;
	double GainBest;
};
```
When a prediction is made, every day's predicted close and actual close is paired up and saved in `Values`. Additionally, the gain through the specified period of time following the algorithm is contained in `GainWith`, the gain through the specified period of time by simply buying at the beginning and selling at the ending is continaed in `GainWithout`, and the gain through the specified period of time if you actually knew the following day's closing price and bought and sold accordingly is contained in `GainBest`.

### Training and Predicting
```C++
PredictionInformation * TrainPredict(int epochs, int minibatchsize, double learningrate, string startingdate, int numberofdates, int numberofiterations);
```
This method combines training and predicting steps for a set number of iterations to simulate an extended trading strategy. `epochs`, `minibatchsize`, and `learningrate` all assume their traditional values given in the `ArtificialNeuralNetwork`. `startingdate` specified the first date to being training on in year-month-day format (2016-01-01). The `numberofdates` specifies how long the training and predicting periods are and `numberofiterations` specifies how many times to repeat training/predicting. Note: the first iteration is ONLY used to train as it wouldn't make sense to predict the data use to train.

### Training
```C++
void Train(int epochs, int minibatchsize, double learningrate, string startingdate, int numberofdates);
```
This method trains the `NeuralNetwork` using the specified `epochs`, `minibatchsize`, and `learningrate` (with their usuall meanings). `startingdate` specified the first date to being training on in year-month-day format (2016-01-01). The `numberofdates` specifies how long the training and predicting periods are 

### Predicting
```C++
PredictionInformation * Predict(string startingdate, int numberofdates);
```
Using the (presumably) trained `NeuralNetwork`, a prediction starting on `startingdate` (2016-01-01) running for `numberofdates` is made.

### Deinitialization
```C++
~Predictor();
```
The `Predictor` interface must close the connection to the server before the object is deleted. This is the purpose of the destructor.

### Example
```C++
#include "Stock/StockPredictor.h"
#include <iostream>

int main()
{
	NeuralDimensions sizes = { 5, 100, 100, 100, 1 };

	StockPredictor stockPredictor(sizes, "AAPL", "<HOSTNAME>", "<USERNAME>", "<PASSWORD>");

	PredictionInformation * predictionInfo = stockPredictor.Predict("2016-02-05", 14);
	cout << predictionInfo->GainWith << " " << predictionInfo->GainWithout << " " << predictionInfo->GainBest << endl;

	stockPredictor.Train(500, 7, 0.5, "2016-01-01", 35);
	
	predictionInfo = stockPredictor.Predict("2016-02-05", 14);
	cout << predictionInfo->GainWith << " " << predictionInfo->GainWithout << " " << predictionInfo->GainBest << endl;

	for (int i = 0; i < 50; i++)
	{
		predictionInfo = stockPredictor.TrainPredict(500, 7, 0.5, "2016-01-29", 7, 2);
		cout << predictionInfo->GainWith << " " << predictionInfo->GainWithout << " " << predictionInfo->GainBest << endl;
	}
	
	return 0;
}
```
This simple example is what I am currently using to determine proper `NeuralDimensions` and training parameters. The `startingdate`s are set so that training occurs on dates that are immediately before the predicting periods. Note: the results produced in the for-loop are not realistic as you cannot train/predict on the same data repeatedly because, in the real world, you wouldn't have that data yet. This is only used to show the effects of the training parameters.

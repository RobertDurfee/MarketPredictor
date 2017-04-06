# MarketPredictor
C++ classes using feed-forward neural networks to predict stocks, treasuries, commodities, and currencies.

### Disclaimer
This is not production-worthy code! View this simply as a proof-of-concept.

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

### Typedefs
```C++
typedef vector<double> Predictions;
typedef vector<double> Actuals;
typedef vector<Predictions /* Acutals */> PredictionActualPair;
typedef vector<PredictionActualPair> PredictionSet;
```

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

### Training and Predicting
```C++
PredictionInformation * TrainPredict(int epochs, int minibatchsize, double learningrate, string startingdate, int numberofdates, int numberofiterations);
```

### Training
```C++
void Train(int epochs, int minibatchsize, double learningrate, string startingdate, int numberofdates);
```

### Predicting
```C++
PredictionInformation * Predict(string startingdate, int numberofdates);
```

### Deinitialization
```C++
~Predictor();
```
The `Predictor` interface must close the connection to the server before the object is deleted. This is the purpose of the destructor.

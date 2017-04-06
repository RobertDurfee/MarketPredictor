#ifndef PREDICTOR_TYPES_HEADER
#define PREDICTOR_TYPES_HEADER

#include <string> //std::string
#include <vector> //std::vector<>

typedef std::string Date;
typedef std::vector<Date> Dates;

#define PREDICTION 0
#define ACTUAL     1

typedef std::vector<double> Predictions;
typedef std::vector<double> Actuals;
typedef std::vector<Predictions /* Acutals */> PredictionActualPair;
typedef std::vector<PredictionActualPair> PredictionSet;

#endif
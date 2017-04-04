#ifndef ARTIFICIAL_NEURAL_NETWORK_HEADER
#define ARTIFICIAL_NEURAL_NETWORK_HEADER

#include <armadillo> //Col, Mat, set_seed()
#include <vector>    //vector, swap()
#include <random>    //default_random_engine, uniform_int_distribution
#include <chrono>    //system_clock
#include <fstream>   //ifstream, ofstream

#pragma comment(lib, "lapack_win64_MT.lib")
#pragma comment(lib, "blas_win64_MT.lib")

using namespace std;
using namespace arma;

typedef vector<int> NeuralDimensions;
typedef vector<vector<Col<double>>> NeuralData;
typedef vector<Col<double>> NeuralDataPoint;
typedef vector<Col<double>> NeuralBiases;
typedef vector<Mat<double>> NeuralWeights;
typedef Col<double> NeuralOutput;
typedef Col<double> NeuralInput;
typedef Col<double> NeuralError;

NeuralOutput ActiviationFunction(NeuralInput z)
{
	return 1.0 / (1.0 + exp(-z));
}
NeuralOutput ActivationFunctionPrime(NeuralInput z)
{
	return ActiviationFunction(z) % (1 - ActiviationFunction(z));
}

class NeuralNetwork
{
public:
	NeuralNetwork(NeuralDimensions sizes, void(*Evaluate)(NeuralNetwork *, NeuralData));
	NeuralNetwork(string filename, void(*Evaluate)(NeuralNetwork *, NeuralData));

	NeuralOutput FeedForward(NeuralInput a);
	void StochasticGradientDescent(NeuralData trainingData, int epochs, int miniBatchSize, double eta, NeuralData testData);

	void Save(string filename);
	void Open(string filename);

	int GetEpoch();
	void SetSizes(NeuralDimensions sizes);

private:
	int Epoch;
	int NumberOfLayers;
	NeuralDimensions sizes;
	NeuralBiases b;
	NeuralWeights w;
	NeuralBiases empty_b;
	NeuralWeights empty_w;
	default_random_engine random;

	void(*Evaluate)(NeuralNetwork *, NeuralData);

	void Backpropogation(NeuralInput x, NeuralOutput y, NeuralBiases * delta_nabla_b, NeuralWeights * delta_nabla_w);

	vector<NeuralData> SplitIntoMiniBatches(NeuralData trainingData, int miniBatchSize);
};

NeuralNetwork::NeuralNetwork(NeuralDimensions sizes, void(*Evaluate)(NeuralNetwork *, NeuralData) = NULL)
{
	arma_rng::set_seed((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
	random.seed((unsigned int)chrono::system_clock::now().time_since_epoch().count());

	SetSizes(sizes);

	this->Evaluate = Evaluate;
	Epoch = 0;
}
NeuralNetwork::NeuralNetwork(string filename, void(*Evaluate)(NeuralNetwork *, NeuralData) = NULL)
{
	arma_rng::set_seed((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
	random.seed((unsigned int)chrono::system_clock::now().time_since_epoch().count());

	Open(filename);

	this->Evaluate = Evaluate;
	Epoch = 0;
}

NeuralOutput NeuralNetwork::FeedForward(NeuralInput a)
{
	for (unsigned int l = 0; l < b.size(); l++)
		a = ActiviationFunction((w[l] * a) + b[l]);

	return a;
}
void NeuralNetwork::StochasticGradientDescent(NeuralData trainingData, int epochs, int miniBatchSize, double eta, NeuralData testData = NeuralData())
{
	Epoch = 0;

	if (testData.size() > 0 && Evaluate != NULL)
		Evaluate(this, testData);

	int n = (int)trainingData.size();
	for (Epoch = 1; Epoch <= epochs; Epoch++)
	{
		vector<NeuralData> miniBatches = SplitIntoMiniBatches(trainingData, miniBatchSize);

		for (unsigned int miniBatch = 0; miniBatch < miniBatches.size(); miniBatch++)
		{
			NeuralBiases nabla_b = empty_b;
			NeuralWeights nabla_w = empty_w;

			for (unsigned int trainingExample = 0; trainingExample < miniBatches[miniBatch].size(); trainingExample++)
			{
				NeuralBiases delta_nabla_b = empty_b;
				NeuralWeights delta_nabla_w = empty_w;

				Backpropogation(miniBatches[miniBatch][trainingExample][0], miniBatches[miniBatch][trainingExample][1], &delta_nabla_b, &delta_nabla_w);

				for (unsigned int l = 0; l < nabla_b.size(); l++)
				{
					nabla_b[l] = nabla_b[l] + delta_nabla_b[l];
					nabla_w[l] = nabla_w[l] + delta_nabla_w[l];
				}
			}

			for (unsigned int l = 0; l < b.size(); l++)
			{
				b[l] = b[l] - (eta / miniBatches[miniBatch].size()) * nabla_b[l];
				w[l] = w[l] - (eta / miniBatches[miniBatch].size()) * nabla_w[l];
			}
		}

		if (testData.size() > 0 && Evaluate != NULL)
			Evaluate(this, testData);
	}
}

void NeuralNetwork::Open(string filename)
{
	ifstream ifile(filename, ios::binary);

	ifile.read((char *)&NumberOfLayers, sizeof(int));

	for (int l = 0; l < NumberOfLayers; l++)
	{
		int temp;
		ifile.read((char *)&temp, sizeof(int));
		sizes.push_back(temp);
	}

	for (int l = 0; l < NumberOfLayers - 1; l++)
	{
		b.push_back(Col<double>(sizes[l + 1]));
		for (int j = 0; j < sizes[l + 1]; j++)
			ifile.read((char *)&b[l](j), sizeof(double));
		empty_b.push_back(Col<double>(sizes[l + 1], fill::zeros));
	}

	for (int l = 0; l < NumberOfLayers - 1; l++)
	{
		w.push_back(Mat<double>(sizes[l + 1], sizes[l]));
		for (int j = 0; j < sizes[l + 1]; j++)
			for (int k = 0; k < sizes[l]; k++)
				ifile.read((char *)&w[l](j, k), sizeof(double));
		empty_w.push_back(Mat<double>(sizes[l + 1], sizes[l], fill::zeros));
	}

	ifile.close();
}
void NeuralNetwork::Save(string filename)
{
	ofstream ofile(filename, ios::binary);

	ofile.write((char *)&NumberOfLayers, sizeof(int));

	for (int l = 0; l < NumberOfLayers; l++)
		ofile.write((char *)&sizes[l], sizeof(int));

	for (int l = 0; l < NumberOfLayers - 1; l++)
		for (int j = 0; j < sizes[l + 1]; j++)
			ofile.write((char *)&b[l](j), sizeof(double));

	for (int l = 0; l < NumberOfLayers - 1; l++)
		for (int j = 0; j < sizes[l + 1]; j++)
			for (int k = 0; k < sizes[l]; k++)
				ofile.write((char *)&w[l](j, k), sizeof(double));

	ofile.close();
}

int NeuralNetwork::GetEpoch()
{
	return Epoch;
}
void NeuralNetwork::SetSizes(NeuralDimensions sizes)
{
	NumberOfLayers = (int)sizes.size();
	this->sizes = sizes;

	b.clear();
	empty_b.clear();
	w.clear();
	empty_w.clear();

	for (int l = 1; l < NumberOfLayers; l++)
	{
		b.push_back(Col<double>(sizes[l], fill::randn));
		empty_b.push_back(Col<double>(sizes[l], fill::zeros));
		w.push_back(Mat<double>(sizes[l], sizes[l - 1], fill::randn));
		empty_w.push_back(Mat<double>(sizes[l], sizes[l - 1], fill::zeros));
	}
}

void NeuralNetwork::Backpropogation(NeuralInput x, NeuralOutput y, NeuralBiases * delta_nabla_b, NeuralWeights * delta_nabla_w)
{
	NeuralOutput a = x;
	vector<NeuralOutput> as; as.push_back(a);
	NeuralError z;
	vector<NeuralError> zs;

	for (unsigned int l = 0; l < b.size(); l++)
	{
		z = (w[l] * a) + b[l];
		zs.push_back(z);
		a = ActiviationFunction(z);
		as.push_back(a);
	}

	NeuralError delta = (as[as.size() - 1] - y) % ActivationFunctionPrime(zs[zs.size() - 1]);
	(*delta_nabla_b)[delta_nabla_b->size() - 1] = delta;
	(*delta_nabla_w)[delta_nabla_w->size() - 1] = delta * as[as.size() - 2].t();

	for (int l = (int)delta_nabla_b->size() - 2; l >= 0; l--)
	{
		delta = (w[l + 1].t() * delta) % ActivationFunctionPrime(zs[l]);
		(*delta_nabla_b)[l] = delta;
		(*delta_nabla_w)[l] = delta * as[l].t();
	}
}

vector<NeuralData> NeuralNetwork::SplitIntoMiniBatches(NeuralData trainingData, int miniBatchSize)
{
	uniform_int_distribution<int> discrete(0, (int)trainingData.size() - 1);

	vector<NeuralData> Output;

	for (unsigned int index = 0; index < trainingData.size() - 1; index++)
		swap(trainingData[discrete(random)], trainingData[index]);

	if (miniBatchSize > (int)trainingData.size())
		miniBatchSize = (int)trainingData.size();

	int NumberOfMiniBatches = (int)trainingData.size() / miniBatchSize;
	for (int MiniBatchIndex = 0; MiniBatchIndex < NumberOfMiniBatches; MiniBatchIndex++)
	{
		Output.push_back(NeuralData());
		for (int i = MiniBatchIndex; i < MiniBatchIndex + miniBatchSize; i++)
			Output[MiniBatchIndex].push_back(trainingData[i]);
	}

	return Output;
}

#endif
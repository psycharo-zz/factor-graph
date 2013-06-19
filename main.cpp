#include <iostream>
#include <fstream>
using namespace std;


#include <variable.h>
#include <gaussian.h>
#include <gamma.h>
#include <mixture.h>
#include <discrete.h>

#include <examples.h>

#include <algonquin.h>

using namespace vmp;
using namespace algonquin;


std::vector<std::vector<double> > read_bins(const std::string &fileName)
{
    std::vector<std::vector<double> > result;

    std::ifstream infile(fileName);
    std::string line;
    std::getline(infile, line);

    size_t count = atoi(line.c_str());
    while (infile.good())
    {
        std::vector<double> curr(count, 0);

        for (size_t i = 0; i < count; ++i)
        {
            std::getline(infile, line, ',');
            curr[i] = atof(line.c_str());
        }
        result.push_back(curr);
    }

    return result;
}



void testSpeechGMM()
{
    auto speechBins = read_bins("speechbin.txt");

    cout << speechBins.size() << endl;
    vector<double> bin = speechBins[0];

    const size_t numPoints = 500;
    const size_t maxNumIters = 500;
    const size_t numMixtures = 8;

    double evidence;
    Parameters<MoG> params = trainGMM(bin.data(), numPoints,
                                      maxNumIters, numMixtures,
                                      1, GaussianParameters(0, 1e-3), GammaParameters(1e-3, 1e-3),
                                      evidence);
    cout << params << endl;
}



// algonquin for a single frequency band
void testAlgonquin()
{
    // priors
    vector<double> weights = {0.4991, 0.2737, 0.0, 0.2270};
    vector<double> precs = {0.5482, 0.9289, 0.0222, 0.8215};
    vector<double> meanPrecs = {4.2749, -3.4840,-6.4307, 0.0516};

    Parameters<MoG> speechPriors;
    speechPriors.weights = weights;
    speechPriors.components.resize(weights.size());

    for (size_t m = 0; m < weights.size(); ++m)
    {
        speechPriors.components[m].meanPrecision = meanPrecs[m];
        speechPriors.components[m].precision = precs[m];
        cout << meanPrecs[m] / precs[m] << " " << weights[m] << endl;
    }

    Parameters<MoG> noisePriors;
    noisePriors.weights = {1};
    noisePriors.components = {GaussianParameters(-1.9787, 0.5141)};

    // noisy speech
    auto noisyBin = read_bins("noisybin.txt")[0];
    auto speechBin = read_bins("speechbin.txt")[0];

    // clean speech

    Network nwk;
    nwk.setPriors(speechPriors, noisePriors);
    for (size_t f = 0; f < noisyBin.size(); ++f)
    {
        pair<double, double> estimated = nwk.process(noisyBin[f]);
        cout << exp(speechBin[f]) << "\t"
             << exp(estimated.first) << "\t"
             << exp(noisyBin[f]) << "\t"
             << endl;
    }
}


int main()
{
    testAlgonquin();


    return 0;
}


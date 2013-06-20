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



/*
 *
points: 200
iters:  466
mixtures:4
evidence:-552.123
means = [4.22319 -3.46179 -6.42874 0.00133329 ]
precs = [0.520569 0.909226 0.0220358 1.01377 ]
weights = [0.509738	0.278057	0.010094	0.202111	]
1.01 seconds.


points: 999
iters:  611
mixtures:4
evidence:-2546.75
means = [-3.98751 0.638693 3.99423 0 ]
precs = [0.265769 0.197848 1.0533 1 ]
weights = [0.255284	0.319829	0.424326	0.000560864	]
7.06 seconds.
 *
 */


void testSpeechGMM()
{
    auto speechBins = read_bins("speechbin.txt");
    vector<double> bin = speechBins[0];

    const size_t numPoints = 999;
    const size_t maxNumIters = 1000;
    const size_t numMixtures = 4;

    double evidence;
    size_t iters;


    clock_t startTime = clock();
    Parameters<MoG> params = trainGMM(bin.data(), numPoints,
                                      maxNumIters, numMixtures,
                                      1, GaussianParameters(0, 1e-3), GammaParameters(1e-3, 1e-3),
                                      evidence,
                                      iters);
    double runTime = double(clock() - startTime) / (double) CLOCKS_PER_SEC;

    cout << "points: " << numPoints << endl
         << "iters:  " << iters << endl
         << "mixtures:" << numMixtures << endl
         << "evidence:" << evidence << endl
         << params
         << runTime << " seconds." << endl;
}



#include <ctime>

#include <gperftools/profiler.h>

int main()
{

    ProfilerStart("gmm.prof");
    testSpeechGMM();
    ProfilerStop();

    // to compute its execution duration in runtime





    return 0;
}


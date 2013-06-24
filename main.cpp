
using namespace std;
#include <iostream>
#include <fstream>
#include <ctime>
#include <gperftools/profiler.h>
#include <cfloat>
#include <vector>

#include <variable.h>
#include <gaussian.h>
#include <gaussianarray.h>
#include <gamma.h>
#include <mixture.h>
#include <discrete.h>
#include <discretearray.h>
#include <examples.h>
#include <algonquin.h>



vector<vector<double> > read_bins(const string &fileName)
{
    vector<vector<double> > result;
    ifstream infile(fileName);
    string line;
    getline(infile, line);
    size_t count = atoi(line.c_str());
    while (infile.good())
    {
        vector<double> curr(count, 0);
        for (size_t i = 0; i < count; ++i)
        {
            getline(infile, line, ',');
            curr[i] = atof(line.c_str());
        }
        result.push_back(curr);
    }
    return result;
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


points: 999
iters:  455
mixtures:4
evidence:-2557.95
means = [0.442826 -3.69693 3.92595 0 ]
precs = [0.421325 0.246173 0.92228 1 ]
weights = [0.209986	0.303197	0.486256	0.000560864	]
0.3 seconds.

 *
 */


using namespace vmp;

void testSpeechGMM()
{
    auto speechBins = read_bins("speechbin.txt");
    vector<double> bin = speechBins[0];

    const size_t numPoints = 999;
    const size_t maxNumIters = 456;
    const size_t numMixtures = 4;

    double evidence = 0;
    size_t iters;


    clock_t startTime = clock();
    Parameters<MoG> params = trainGMM(bin.data(), numPoints,
                                      maxNumIters, numMixtures,
                                      1, GaussianParameters(0, 1e-3), GammaParameters(1e-3, 1e-3),
                                      evidence, iters);
    double runTime = double(clock() - startTime) / (double) CLOCKS_PER_SEC;

    cout << "points: " << numPoints << endl
         << "iters:  " << iters << endl
         << "mixtures:" << numMixtures << endl
         << "evidence:" << evidence << endl
         << params
         << runTime << " seconds." << endl;
}



int main()
{
    testSpeechGMM();


    return 0;
}



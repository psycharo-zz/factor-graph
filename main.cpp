#include <iostream>
#include <fstream>



#include <variable.h>
#include <gaussian.h>
#include <gaussianarray.h>
#include <gamma.h>
#include <mixture.h>
#include <discrete.h>
#include <discretearray.h>
#include <examples.h>
#include <algonquin.h>
#include <ctime>
#include <gperftools/profiler.h>
#include <cfloat>


using namespace std;
using namespace vmp;



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

means = [0.455054 -3.59932 3.91002 0 ]
precs = [0.487944 0.236359 0.908833 1 ]
weights = [0.190794	0.315197	0.493449	0.000560864	]

 *
 */


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


int main()
{
    testSpeechGMM();

//    trainDirichletVector(10);
//    trainGaussianVector(10);
//    trainUnivariateGaussian(10);
//    trainDirichletVector(10);


    return 0;
}


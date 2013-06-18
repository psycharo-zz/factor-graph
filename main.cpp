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




int main()
{
    testSpeechGMM();


    return 0;
}


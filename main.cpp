
using namespace std;
#include <iostream>
#include <fstream>
#include <ctime>
#include <gperftools/profiler.h>
#include <cfloat>
#include <vector>

#include <examples.h>
#include <mvalgonquin.h>



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




int main()
{
    using namespace vmp;
//    vmp::testSpeechGMM(read_bins("speechbin.txt")[0]);

    vector<vec> MU = { {1.0, 2.0},
                       {-3.0, -5.0},
                       {-11.0, -2.0} };
    vector<vec> SIGMA = {{2.0, 0.5},
                         {4.0, 1.0},
                         {1.0, 5.0}};

    vec WEIGHTS = {0.25, 0.5, 0.25};
    size_t numPoints = 1001;
    size_t numMixtures = 6;
    size_t maxNumIters = 40;
    auto POINTS = gmmrand(numPoints, MU, SIGMA, WEIGHTS);

    auto nwk = vmp::trainMVMixture(POINTS, numMixtures, maxNumIters);

    for (size_t m = 0; m < numMixtures; ++m)
    {
        cout << nwk->weights->moments().probs[m] << ":" << nwk->means->moments(m).mean.t() << endl;
        cout << 1. / diagvec(nwk->precs->moments(m).prec).t() << endl;
    }


    delete nwk;

    return 0;
}




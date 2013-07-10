
using namespace std;
#include <iostream>
#include <fstream>
#include <ctime>
#include <gperftools/profiler.h>
#include <cfloat>
#include <vector>

#include <examples.h>
#include <algonquin/mvalgonquin.h>



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


#include <gsl/gsl_randist.h>



int main()
{
    using namespace vmp;
//    testMVMoG();
    vector<vec> MU = { {-3}, {3} };
    vector<vec> SIGMA = { {1}, {1} };
    vec WEIGHTS = {0.4, 0.6};
    vec data = gmmrand(1000, MU, SIGMA, WEIGHTS);
    auto res = trainUnivariateMixture(data.mem, data.n_rows, 4, 200);



/*
    gsl_rng_env_setup();

    const gsl_rng_type *T = gsl_rng_default;
    gsl_rng *r = gsl_rng_alloc(T);

    vec ALPHA = {1,5,6,7,8,9};
    size_t DIMS = ALPHA.size();
    vec THETA(DIMS);


    size_t numPoints = 100;
    vector<Discrete::TParameters> params(numPoints, Discrete::TParameters(DIMS));

    for (size_t i = 0; i < numPoints; ++i)
    {
        gsl_ran_dirichlet(r, DIMS, ALPHA.memptr(), THETA.memptr());
        params[i].logProb = log(THETA);
    }

    auto dir = Dirichlet(DIMS, DIRICHLET_PRIOR);

    auto discr = DiscreteArray(params, &dir);

    auto msg = dir.addChild(&discr);

    for (size_t i = 0; i < 2; ++i)
    {
        discr.messageToParent(msg);
        dir.updatePosterior();
    }
*/


    return 0;
}




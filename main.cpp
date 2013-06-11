#include <iostream>
using namespace std;


#include <variable.h>
#include <gaussian.h>
#include <gamma.h>
#include <mixture.h>
#include <discrete.h>
#include <ostream>
#include <ctime>
#include <limits>


#include <examples.h>

using namespace vmp;



int main()
{
//    trainUnivariateGaussian(10);
   trainMixtureOfUnivariateGaussians(120);
//    trainDirichlet(50);

    return 0;
}


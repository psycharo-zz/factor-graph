#include <iostream>

using namespace std;


#include <variable.h>
#include <gaussian.h>
#include <gamma.h>
#include <network.h>

using namespace vmp;


void trainUnivariateGaussian()
{
    Network nwk;

    Variable *mu = nwk.createVariable<Gaussian>(0, 1e-2);
    Variable *psi = nwk.createVariable<Gamma>(1e-3, 1e-3);

    vector<Variable*> x;

    double data[] = {0.257056,-0.944378,-1.321789,0.924826,0.000050,-0.054919,0.911127,0.594584,0.350201,1.250251};
    vector<double> v(data, data + sizeof(data) / sizeof(double));

    for (int i = 0; i < v.size(); ++i)
    {
        x.push_back(nwk.createVariable<Gaussian>(mu, psi));
        x[i]->observe(data[i]);
    }


    cout << "smthing" << endl;
//    x->observe(v);
    // cout << nwk.runInference(10) << endl;




}

/*
everything is defined as a set of variables connected via factors
every variable can be:
-continuous/discrete
-observed (point mass) / unobserved (the actual distribution)

arrays?
 */



int main()
{
    trainUnivariateGaussian();

    return 0;
}


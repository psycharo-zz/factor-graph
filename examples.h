#ifndef EXAMPLES_H
#define EXAMPLES_H

#include <util.h>
#include <vector>
#include <memory>

#include <variable.h>
#include <gaussian.h>
#include <mixture.h>

template <typename T>
void deleteAll(std::vector<T*> &vec)
{
    for (size_t i = 0; i < vec.size(); ++i)
        delete vec[i];
}


const double DIRICHLET_PRIOR_U = 1;
const double GAMMA_PRIOR_SHAPE = 1e-3;
const double GAMMA_PRIOR_RATE = 1e-3;
const double GAUSS_PRIOR_MEAN = 0;
const double GAUSS_PRIOR_PREC = 1e-3;


const double MIX_DATA[] = {
// mixtures,
0.019209,-0.005372,-0.098024,-0.018035,0.096218,0.060276,-0.227076,0.128158,0.035287,0.267884,-0.157314,
    -0.102112,-0.247797,0.000511,0.140735,-0.067736,-0.073412,-0.041671,-0.022705,0.144747,0.002248,-0.068258,
    0.089757,-0.192961,0.189302,-0.065300,0.676527,-0.205173,0.133952,0.037272,-0.341772,0.276866,-0.704012,
    -0.615414,-0.414130,0.412874,0.311158,-0.395723,-0.056843,-0.235086,0.073758,0.664504,-0.277226,0.616261,
    -0.147132,-0.206154,0.192782,0.224245,0.088479,-0.464050,-0.218614,-0.274482,-0.162069,-0.247317,0.134646,
    0.319877,-0.079268,0.175265,0.239979,0.042325,-0.086192,-0.171594,-0.367806,-0.086494,0.050440,-0.059042,
    0.068907,0.014622,0.211975,-0.199382,-0.037445,0.076548,5.881072,6.088684,5.438869,6.145015,6.146041,5.999624,
    5.888391,5.863076,5.460903,6.346496,6.201049,5.656202,5.532848,5.878173,6.215299,6.263307,6.169055,6.242871,6.554944,
    5.461049,6.148472,5.982828,5.763185,6.226661,5.998257,6.379509,5.673015,5.731976,6.137060,5.719340,6.038337,
    5.872907,6.346800,6.138089,5.871294,5.798028,5.871965,6.757006,5.984308,5.935940,5.937534,6.479501,5.452228,
    6.351795,5.772543,5.913554,6.214214,5.703868,
};
const size_t MIX_NUM_POINTS = sizeof(MIX_DATA) / sizeof(double);


vmp::Parameters<vmp::MoG> trainGMM(const double *trainingData, size_t numPoints, size_t maxNumIters, size_t numMixtures,
                                   double dirichletPrior, const vmp::Parameters<vmp::Gaussian> &priorMean, const vmp::Parameters<vmp::Gamma> &priorGamma,
                                   double &evidence, size_t &iters);





#endif // EXAMPLES_H

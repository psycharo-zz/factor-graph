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



template <typename TParent, typename TChild>
void sendToChild(TParent *parent, TChild *child)
{
    child->receiveFromParent(parent->messageToChildren(), parent);
}

template <typename TParent, typename TChild>
void sendToParent(TChild *child, TParent *parent)
{
    parent->receiveFromChild(child->messageToParent(parent), child);
}



const double DATA[] = {-0.371482,1.763169,-3.592885,-2.263371,-2.180174,-2.290550,4.786776,3.460986,
                       -1.239315,0.883568,-3.307443,-1.411931,-0.584617,0.610866,2.407795,1.234371,
                       -0.039581,2.484930,-0.368116,1.309901,4.068510,-2.340014,-0.561350,-1.958976,
                       -3.137285,1.980247,-0.922968,-3.403374,-2.048146,1.971006,1.293586,0.225300,-3.170316,
                       -1.040818,-1.924622,2.677459,-1.324865,1.702337,3.390877,2.674503,1.243590,-2.579183,4.648140,
                       -2.643176,-1.372459,-0.032691,1.493348,0.546879,-1.606135,-1.989330,-3.003583,-0.762470,-0.575882,
                       -0.280852,-0.762253,1.006738,-0.281002,-1.173300,-0.553786,3.279227,0.130098,2.122636,-0.760417};
const size_t NUM_POINTS = sizeof(DATA) / sizeof(double);


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



const double BIN_DATA[] = {
-0.171657, 0.631317, -0.265972, 0.180574, -1.380882, -0.517581, -0.134073, -0.370156, -1.944621, -1.675596, -0.887551, -0.599127, -1.496188, -2.464563, -1.897255,
-0.665925, -1.770503, -3.162471, -0.750184, -0.562857, -0.458616, 4.380693, 2.193758, 1.861088, 0.425491, 1.682478, 1.857043, 1.836348, 2.195628, 2.722611,
2.500140, 2.184294, 2.267554, 2.299876, 2.153344, 1.724578, 1.332501, 2.006360, 2.061582, 2.260866, 2.689089, 3.008491, 3.139540, 3.205695, 2.831211,
0.403643, -2.720906, -1.454352, -2.420510, -6.178385, -1.466413, -1.929810, -1.796732, 2.164989, 4.641431, 4.369709, 4.550239, 4.389668, 4.341390, 4.044675,
3.991317, 3.828058, 3.795957, 3.649997, 3.727898, 3.744519, 3.846729, 4.141132, 4.200289, 4.213893, 4.232948, 4.169473, 4.155301, 4.007233, 4.093948,
4.027231, 3.903451, 3.640416, 3.558599, 3.502517, 3.374957, 3.187793, 2.778714, 3.288953, 3.548047, 3.017954, 1.545063, 2.513439, 2.635428, 1.705726,
0.614381, 2.549318, 1.635334, 0.063378, 2.394142, 1.594713, -0.561015, 2.164499, 1.089937, 0.168296, 2.073788, 1.356377, -0.329467, 1.850085, 1.622427,
-1.045200, 1.495903, 1.775095, -1.004269, 1.540831, 0.474981, 0.805440, -0.169553, -2.043539, -4.361610, -3.510840, -6.046363, -3.595049, -5.508123, -4.362192,
-0.536676, -0.736429, -4.463425, -3.427137, -2.940090, 0.361126, 3.533843, 4.109821, 4.086451, 4.215668, 4.134801, 4.047751, 4.038591, 3.964445, 3.924562,
3.895031, 3.840573, 3.584628, 3.281626, 3.063134, 2.642310, 2.156803, 1.795710, 1.391326, 0.969854, -1.020526, -1.411159, -3.075451, -3.367133, -3.991006,
-2.601579, -5.384095, -3.882391, -3.700762, -1.535044, 3.215122, 4.527912, 4.708840, 4.699854, 4.381773, 4.408222, 4.416005, 4.355352, 4.359184, 4.268030,
4.174606, 4.173181, 4.142044, 4.077133, 3.995430, 3.951292, 4.008033, 3.995985, 3.968863, 3.796650, 3.750190, 3.786980, 3.583283, 3.768246, 3.846559,
3.926194, 4.004017, 3.934886, 3.539110, 2.359751, -0.455666, -2.922691, -2.011083, -2.432461, -2.600137, -2.382051, -2.733458, -3.935646, -5.062376, -4.152552,
-3.841815, -2.943065, -5.474840, 0.154956, 0.322078, -1.648076, -6.070296, -0.999583, 2.810097, 3.621295, 3.170059, 3.181993, 3.193187, 3.340761, 3.263264,
1.778742, 3.004637, 2.142501, -0.348456, 1.261859, 1.362921, -0.586197, -0.488844, -2.679158, 0.163545, -3.309907, -0.374042, -1.094780, -1.476386, -1.916360,
-1.770205, -3.120877, -6.798474, -4.488033, -5.484837, -4.850907, -4.179535, -4.894175, -4.137969, -4.263053, -3.583103, -3.848281, -4.656626, -4.525101, -5.172701,
-4.415203, -7.326882, -4.505815, -4.842043, -4.670217, -3.914559, -4.109099, -3.455285, -5.753343, -6.071140, -3.221176, -5.944497, -4.425145, -5.461993, -4.803996,
-4.755857, -4.993197, -6.057524, -6.643274, -7.400769, -3.572972, -8.360984, -9.090750, -4.771613, -6.596156, -8.177958, -4.454651, -5.560433, -9.761702, -5.635261,
-4.525852, -5.088811, -4.517129, -3.778796, -3.828806, -6.336290, -7.124937, -5.546250, -5.435081, -5.689038, -4.898191, -4.296876, -4.414278, -4.110778, -4.693214,
-3.864941, -5.252082, -3.641527, -7.537818, -3.917524, -5.581510, -5.529841, -3.877042, -4.073311, -4.449982, -6.415030, -5.791734, -5.548939, -3.451579, -5.815693,
-5.722918, -6.159704, -4.106605, -5.978788, -4.300351, -3.941050, -3.977961, -3.436879, -3.968372, -3.793971, -4.301036, -4.790866, -2.974137, -0.270599, 2.809124,
3.444364, 3.657312, 2.992494, 2.820968, 2.623386, 1.371841, -0.661734, -0.228115, -0.941392, -0.620615, -3.006413, -0.740461, 2.360121, 4.549044, 4.637145,
4.640312, 4.466337, 4.122126, 3.943238, 3.821383, 3.686826, 3.624635, 3.635488, 3.718361, 3.703544, 3.574894, 3.169063, 2.635693, 2.123384, 2.256802,
1.988274, 1.557605, 0.883049, 0.813310, 1.341716, 1.228777, 0.799469, 1.472556, 1.250462, 1.631960, 1.814234, 1.406707, 1.883914, 1.750401, 1.808422,
1.935569, 2.131912, 2.269068, 2.533206, 2.834262, 3.044366, 3.081547, 2.653015, 0.984004, -2.377086, -1.452543, -2.812842, -2.968625, -2.197414, 1.072320,
4.226244, 4.593976, 4.837451, 4.809592, 4.660149, 4.336637, 4.169052, 4.282713, 4.296375, 4.298604, 4.216979, 4.102665, 3.952766, 3.855677, 3.690301,
3.562945, 3.244294, 2.802668, 2.434484, 0.662887, -0.210178, -0.795909, -1.794580, -1.755209, -2.226837, -3.690191, -4.077308, -2.302817, -3.281927, -4.130822,
-1.580720, -0.486192, -1.760564, -6.094625, -3.698724, -2.691888, -3.848949, -6.884733, -3.267788, 3.801228, 4.374579, 4.527938, 4.472198, 4.279490, 4.029015,
4.113281, 4.127031, 4.090671, 3.734841, 3.439401, 3.108604, 3.257599, 3.718145, 3.842871, 2.954508, 2.769872, 3.297191, 2.780996, 0.574381, 1.732341,
2.949816, 2.109333, 0.571181, 2.657800, 2.509727, -5.196298, 2.757097, 2.855938, 0.488869, 1.794456, 3.013043, 2.636950, -0.128133, 2.594443, 2.992155,
2.214407, 1.107230, 2.925925, 2.530864, 2.652140, 2.636385, 1.132197, 2.259687, 1.679415, 0.874179, 2.722457, 3.045650, 3.533123, 3.632753, 3.720105,
3.748235, 3.713837, 3.354564, 2.699172, 2.659348, 2.482466, 1.030844, 0.310082, 0.494272, 1.583614, 1.680132, 1.202077, 0.688077, -0.358126, -0.464400,
0.351873, 1.497629, 2.886324, 3.752822, 3.835644, 3.579303, 3.338886, 3.096562, 3.153495, 3.208396, 3.257146, 3.283617, 3.482947, 3.414631, 3.453410,
3.476143, 3.626410, 3.233104, 2.516557, 1.309009, 0.711044, 0.614026, -0.162514, -2.167055, -1.293278, -1.765274, 0.159227, 4.077874, 4.374735, 4.137542,
3.601330, 2.729903, 2.085523, 1.639462, 2.175946, 1.496102, -0.141567, 3.404460, 3.917278, 3.986896, 3.768729, 3.620911, 3.520944, 3.374081, 3.284813,
3.512382, 3.563574, 3.548617, 3.487057, 3.311921, 3.239878, 3.005500, 2.549464, 2.400745, 2.598810, 1.521340, 1.884377, 2.999005, 3.772295, 4.151964,
4.123196, 4.222161, 4.220470, 3.971970, 4.087949, 3.477364, 3.269255, 2.376913, 1.620468, -0.277764, -0.462449, -1.964089, -1.748306, -1.631084, -2.252663,
-4.891847, -2.495315, -5.005853, -5.111364, 2.304901, 5.031544, 4.698987, 4.658412, 4.417988, 4.607339, 4.699416, 4.709483, 4.781280, 4.815689, 4.898380,
4.943832, 5.052485, 4.993330, 4.924478, 4.958086, 4.874616, 4.779755, 4.642864, 4.596995, 4.620548, 4.740584, 4.568039, 4.479544, 4.343771, 4.222061,
4.032702, 3.954006, 3.755654, 3.579325, 3.397880, 3.376899, 3.257903, 3.253477, 3.459434, 3.644679, 3.713108, 3.395813, 3.442522, 3.553445, 3.358268,
3.276640, 3.295325, 3.405250, 3.568594, 3.585073, 3.452236, 3.254607, 2.964172, 3.035829, 3.210336, 3.351082, 3.574079, 3.697867, 3.790635, 3.727162,
3.590532, 3.319233, 3.285728, 3.184410, 3.202677, 3.005654, 2.780772, 2.385345, 1.386174, 0.790568, 1.003961, 0.557073, 1.842773, 2.748629, 3.087693,
3.744922, 4.265369, 4.090383, 4.480867, 4.736106, 4.787126, 4.799223, 4.668454, 4.454055, 3.961293, 3.543902, 2.798477, 2.011053, 1.937163, 1.088647,
0.210012, -1.589419, -2.055598, -1.485960, -2.061734, -2.510422, -1.931403, -2.059546, -3.528900, -1.307070, 2.825866, 5.117095, 5.153257, 4.880248, 4.545664,
4.230704, 4.031744, 3.907422, 3.934332, 3.898575, 4.009730, 3.995801, 3.827364, 3.887872, 3.852274, 3.578439, 3.287477, 2.983814, 2.696329, 2.548911,
2.369128, 2.199565, 2.220965, 2.182357, 2.076037, 1.741567, 1.678386, 1.347247, 1.384965, 1.608661, 1.591584, 1.420875, 1.255705, 1.430347, 1.765130,
1.717414, 2.345207, 2.317643, 2.637066, 2.810494, 3.328892, 3.709929, 3.930713, 4.308807, 4.511569, 4.533024, 4.502458, 4.152081, 3.753751, 3.286752,
2.541195, 2.131603, 1.371619, 1.594659, 1.641420, -1.052798, 0.506783, -1.294825, -1.915440, -2.331940, -1.474703, -0.033569, 1.635492, 2.570490, 3.112819,
3.341847, 3.773182, 4.261370, 4.547669, 4.649351, 4.737355, 4.719410, 4.624526, 4.600511, 4.575524, 4.704542, 4.678837, 4.612130, 4.508704, 4.045632,
3.456128, 1.289785, -0.251376, -2.166543, -2.607941, -0.494743, -1.764883, -2.644139, -2.654357, -3.854432, -3.141213, -0.570540, -0.057334, 3.877763, 3.937082,
4.139620, 4.061302, 4.238142, 4.317741, 4.469308, 4.604288, 4.362436, 4.433985, 4.483806, 4.284007, 4.324615, 4.278141, 4.180408, 3.982760, 4.069613,
4.149878, 4.221850, 4.296973, 4.296375, 4.172218, 4.117748, 4.030675, 4.024011, 3.897616, 3.948487, 3.852521, 3.773782, 3.626555, 3.554941, 3.469874,
3.304007, 2.852475, 2.124323, 2.368386, 2.561509, 2.934442, 3.675743, 3.827177, 3.760705, 3.684264, 2.798217, 2.517199, 2.947454, 2.588980, 1.574748,
1.275113, -0.963125, -1.923815, -1.719174, -2.057471, -1.702792, -1.523229, -0.505404, 3.830495, 3.817376, 3.866614, 3.799544, 3.923532, 4.113652, 4.431796,
4.432853, 4.572849, 4.582239, 4.531920, 4.535390, 4.601662, 4.704245, 4.780608, 4.766171, 4.531597, 3.454516, 0.473738, 1.179425, -1.503607, -0.082633,
-2.472339, -2.782111, -2.085708, -2.892052, -3.711617, -3.576808, -2.548420, -2.608837, -1.663482, -3.263233, -2.396408, -2.715368, 3.205411, 4.507265, 4.017011,
3.919632, 3.766632, 3.568740, 3.686851, 3.817882, 3.670341, 3.057245, 1.942183, 0.436080, -0.087797, -0.699581, -1.897029, -2.132976, 2.196015, 4.404514,
4.697627, 4.725672, 4.616132, 4.075906, 3.228283, 3.056656, 2.191885, 0.438683, -0.214950, 0.368387, 0.920098, 3.669028, 4.622336, 4.653276, 4.536489,
4.518134, 4.455907, 4.384012, 4.240772, 4.070990, 3.790432, 3.197778, 2.580523, 2.262576, 1.851377, 1.245071, -0.170671, -1.305668, -0.708561, -2.219835,
-1.942889, -2.890744, -1.837500, -3.053223, -1.749185, 2.570210, 4.562427, 4.839657, 4.634744, 4.572233, 4.517556, 4.388628, 4.335705, 4.228557, 4.272068,
4.196578, 4.156865, 4.046711, 3.951591, 3.510567, 3.252711, 3.059842, 2.450792, 1.447793, 0.832775, -0.355649, -3.471283, -2.146206, -3.375130, 0.741600,
2.955680, 4.402308, 4.024555, 3.826116, 3.846164, 3.698663, 3.603481, 3.653285, 3.840923, 4.138378, 4.362061, 3.806211, 2.558722, 2.195168, 1.454070,
0.628397, 0.190515, -0.937502, -3.505594, 3.389820, 4.670662, 4.790643, 4.722080, 4.824478, 4.896389, 4.839836, 4.512416, 3.890396, 2.600581, 0.295698,
-1.866093, -0.577606, -0.821900, -1.794756, -2.383079, -3.912270, -5.250143, -3.451850, -4.600650, -4.280521, -4.123795, -3.490203, -3.450892, -2.941038, 3.401516,
3.718354, 3.205864, 3.534941, 3.922574, 4.246407, 4.484526, 4.601800, 4.700317, 4.663067, 4.774299, 4.486509, 4.377968, 4.223170, 4.147861, 4.070905,
3.579806, 3.345841, 3.199252, 3.726963, 4.184241, 3.544319, 2.442310, 3.362355, 2.859299, 0.012784, 1.734022, 2.700732, 0.375794, 1.828956, 2.037349,
-1.433017, 1.413722, 0.776547, -0.189613, 1.281898, -2.618018, 0.981035, -2.180626, 0.783533
};
const size_t BIN_NUM_POINTS = sizeof(BIN_DATA) / sizeof(double);




void trainDirichlet(const size_t maxNumIters);

void trainUnivariateGaussian(const size_t maxNumIters);

vmp::Parameters<vmp::MoG> trainGMM(const double *trainingData, size_t numPoints, size_t maxNumIters, size_t numMixtures,
                                   double dirichletPrior,
                                   const vmp::GaussianParameters &priorMean,
                                   const vmp::GammaParameters &priorGamma,
                                   double &evidence,
                                   size_t &iters);




#endif // EXAMPLES_H
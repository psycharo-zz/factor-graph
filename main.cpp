
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
//    vmp::testMVMoG();




    return 0;
}




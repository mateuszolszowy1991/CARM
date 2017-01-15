#ifndef DSSALGORITHM_H
#define DSSALGORITHM_H
#include <string>
#include <vector>
#include <iostream>
#include "Logger.hpp"
using namespace std;

class DSSAlgorithm
{
    public:
        DSSAlgorithm();
        virtual ~DSSAlgorithm();
        vector<string> calculate(vector<vector<string>>* routes);
    protected:

    private:
};

#endif // DSSALGORITHM_H

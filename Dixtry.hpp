#ifndef DIXTRY_H
#define DIXTRY_H
#include <vector>
#include <iostream>
#include <cmath>
#include "Logger.hpp"

using namespace std;

class Dixtry
{
    public:
        Dixtry();
        virtual ~Dixtry();
        vector<double> scheduleRoot(vector<pair<double, double>> targets);
    protected:

    private:
        vector<double> equalDistances(vector<pair<double, double>> targets);
};

#endif // DIXTRY_H

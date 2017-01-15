#include "Dixtry.hpp"

Dixtry::Dixtry()
{
    //ctor
}

Dixtry::~Dixtry()
{
    //dtor
}

vector<double> Dixtry::scheduleRoot(vector<pair<double, double>> targets)
{
    Logger::saveToFile("CarManager/INF: GpsAgent: Dixtry::scheduleRoot");
    vector<double> distances = equalDistances(targets);
    //next step of algorithm
    return distances;
}

vector<double> Dixtry::equalDistances(vector<pair<double, double>> targets)
{
    vector<int> distances;
    for(const auto &target : targets)
    {
        distances.push_back(sqrt(pow(target.first, 2) + pow(target.second, 2)));
    }
}

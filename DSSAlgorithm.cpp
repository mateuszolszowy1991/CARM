#include "DSSAlgorithm.hpp"

DSSAlgorithm::DSSAlgorithm()
{
    //ctor
}

DSSAlgorithm::~DSSAlgorithm()
{
    //dtor
}

vector<string> DSSAlgorithm::calculate(vector<vector<string>>* routes)
{
    vector<string> result = (*routes)[0];
    for(const auto &route : *routes)
    {
        for(const auto &element : route)
        {
            //cout << "Route: " << element << endl;
        }
    }
    Logger::saveToFile("CARM/INF: DSSAlgorithm: calculated");
    return result;
}

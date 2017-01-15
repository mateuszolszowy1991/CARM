#include "GpsAgent.hpp"


GpsAgent::GpsAgent(IDBWrapper* wrapper) : wrapper_(wrapper), dss_(new DSSAlgorithm())
{
    Logger::saveToFile("CARM/INF GpsManager ctor" );
}

GpsAgent::~GpsAgent()
{
    //dtor
}

bool GpsAgent::createGpsMo()
{
    gpsMo_ = wrapper_->createObject("CAR/GPS", "Online", "Disabled");
    return true;
}

void GpsAgent::setCarPosition()
{
    string location = getLocation();
    vector<string> splittedLocation = splitString(location, '\n');
    Logger::saveToFile(("CARM/INF: GpsAgent: setCarPosition: " + splittedLocation[0] + ", " + splittedLocation[1]).c_str());
    vector<string> positions = {"positionX", "positionY"};
    vector<string> values = {splittedLocation[0], splittedLocation[1]};
    wrapper_->expandObject(&gpsMo_, positions, values);

}

void GpsAgent::waitForNavigateRequest()
{
    Mo mo = wrapper_->getObject("CAR/GPS/NAVIGATE_REQ");
    Logger::saveToFile("CARM/DBG: GpsAgent: Mo: " + mo.distname);
    //Logger::saveToFile("CARM/DBG: GpsAgent: waitForNavigateRequest: distname has been found: " + mo.distname);
    if(mo.distname == "CAR/GPS/NAVIGATE_REQ")
    {
        Logger::saveToFile("CARM/INF: GpsAgent: NAVIGATION_REQ has been found");
        string value = wrapper_->getDetailValue(mo.distname, "file");
        Logger::saveToFile("RETURNED VALUE: " + value);
        if(value != "0")
        {
            mo.details.push_back(pair<string, string>("file", value));
            createTargetsTopology(mo);
            createRoutesTopology();
            wrapper_->deleteObject(mo.distname);
        }
    }
}



void GpsAgent::createTargetsTopology(Mo navigateReqMo)
{
    targetsMo_ = wrapper_->createObject("CAR/GPS/TARGETS", "NA", "NA");
    string line;
    vector<string> sline;
    int i = 0;
    int j = 0;
    string filepath = navigateReqMo.details[0].second;
    fstream file;
    file.open(filepath, ios::in | ios::out);
    if(verifyResultFile(&file))
    {
        while(!file.eof())
        {
            getline(file, line);

            if(isTargetLine(line))
            {
                i++;
                targetMos_.push_back(wrapper_->createObject("CAR/GPS/TARGETS/TARGET_" + to_string(i), "NA", "NA"));
            }
            else if(line == "")
            {
                break;
            }
            else
            {
                sline = splitString(line, ';');
                cout << sline[0] << endl;
                wrapper_->expandObject(&(targetMos_.back()), {"startLocation",
                                                                "endLocation",
                                                                "duration",
                                                                "distance",
                                                                "traffic"},
                                                                {sline[0],
                                                                sline[1],
                                                                sline[2],
                                                                sline[3],
                                                                sline[4]});
            }
        }
    }
}

void GpsAgent::createRoutesTopology()
{
    routesMo_ = wrapper_->createObject("CAR/GPS/ROUTES", "Online", "Calculating");
    calculateProperties();
    calculateNecessaryFuel();
    selectRoutesWithConstrains();
    Mo optimalRoute = calculateDecisionAndMinimizeConstrainedParam();
    Logger::saveToFile(("CARM/INF: GpsAgent: Optimal route is " + optimalRoute.distname).c_str());
    Mo paretoOptimalRoute = calculateParetoDominium();
    Logger::saveToFile(("CARM/INF: GpsAgent:Pareto optimal route is " + paretoOptimalRoute.distname).c_str());
    if( optimalRoute.distname == paretoOptimalRoute.distname)
    {
        addParetoDetailValueForObject(paretoOptimalRoute.distname);
        setResult(paretoOptimalRoute.distname);
    }
    else
    {
        addParetoDetailValueForObject(paretoOptimalRoute.distname);
        addParamterizedDetailValueForObject(optimalRoute.distname);
    }
}

void GpsAgent::setResult(string distname)
{
    Mo result = wrapper_->createObject("CAR/GPS/RESULT", "NA", "NA");
    wrapper_->expandObject(&result, {"selectedRoute"}, {distname});
}

void GpsAgent::addParetoDetailValueForObject(string distname)
{
    for(auto route : routeMos_)
    {
        if(route.distname == distname)
        {
            wrapper_->expandObject(&route, {"paretoOptimal"}, {"true"});
            break;
        }
    }
}

void GpsAgent::addParamterizedDetailValueForObject(string distname)
{
    for(auto route : routeMos_)
    {
        if(route.distname == distname)
        {
            wrapper_->expandObject(&route, {"optimal"}, {"true"});
            break;
        }
    }
}

void GpsAgent::calculateProperties()
{
    Logger::saveToFile("CARM/DBG: GpsAgent: calculateProperties ");
    calculatePropertiesForRouteABCDE();
    calculatePropertiesForRouteACBDE();
    calculatePropertiesForRouteACDBE();
    calculatePropertiesForRouteABDCE();
    calculatePropertiesForRouteADBCE();
    calculatePropertiesForRouteABECD();
    calculatePropertiesForRouteAEBCD();
}

//In future should be set to another class
void GpsAgent::calculatePropertiesForRouteABCDE()
{
    Logger::saveToFile("CARM/DBG: GpsAgent: calculatePropertiesForRouteABCDE ");
    int duration = calculateDuration({1, 5, 8, 10});
    int distance = calculateDistance({1, 5, 8, 10});
    float traffic = calculateTraffic({1, 5, 8, 10});
    Logger::saveToFile("CARM/DBG: GpsAgent: duration" + changeIntToString(duration));
    routeMos_.push_back(wrapper_->createObject("CAR/GPS/ROUTES/ROUTE_1", "Calculated", "Enabled"));
    wrapper_->expandObject(&(routeMos_.back()),
                            {"type", "duration", "distance", "traffic"},
                            {"ABCDE",
                            changeIntToString(duration),
                            changeIntToString(distance),
                            changeFloatToString(traffic)});
}

void GpsAgent::calculatePropertiesForRouteACBDE()
{
    int duration = calculateDuration({2, 5, 6, 10});
    int distance = calculateDistance({2, 5, 6, 10});
    float traffic = calculateTraffic({2, 5, 6, 10});
    routeMos_.push_back(wrapper_->createObject("CAR/GPS/ROUTES/ROUTE_2", "Calculated", "Enabled"));
    wrapper_->expandObject(&(routeMos_.back()),
                            {"type", "duration", "distance", "traffic"},
                            {"ACBDE",
                            changeIntToString(duration),
                            changeIntToString(distance),
                            changeFloatToString(traffic)});
}

void GpsAgent::calculatePropertiesForRouteACDBE()
{
    int duration = calculateDuration({2, 8, 6, 7});
    int distance = calculateDistance({2, 8, 6, 7});
    float traffic = calculateTraffic({2, 8, 6, 7});
    routeMos_.push_back(wrapper_->createObject("CAR/GPS/ROUTES/ROUTE_3", "Calculated", "Enabled"));
    wrapper_->expandObject(&(routeMos_.back()),
                            {"type", "duration", "distance", "traffic"},
                            {"ACBDE",
                            changeIntToString(duration),
                            changeIntToString(distance),
                            changeFloatToString(traffic)});
}
/*It is exactly the same as ROUTE_2 conseqently in the same direction
void GpsAgent::calculatePropertiesForRouteACDEB()
{
    int duration = calculateDuration({2, 8, 6, 7});
    int distance = calculateDistance({2, 8, 6, 7});
    float traffic = calculateTraffic({2, 8, 6, 7});
    routeMos_.push_back(wrapper_->createObject("CAR/GPS/ROUTES/ROUTE_3", "Calculated", "Enabled"));
    wrapper_->expandObject(&(routeMos_.back()), {"type", "duration", "distance", "traffic"}, {"ACBDE", duration.c_str(), distance.c_str(), traffic.c_str()});
}
*/
void GpsAgent::calculatePropertiesForRouteABDCE()
{
    int duration = calculateDuration({1, 6, 8, 9});
    int distance = calculateDistance({1, 6, 8, 9});
    float traffic = calculateTraffic({1, 6, 8, 9});
    routeMos_.push_back(wrapper_->createObject("CAR/GPS/ROUTES/ROUTE_4", "Calculated", "Enabled"));
    wrapper_->expandObject(&(routeMos_.back()),
                            {"type", "duration", "distance", "traffic"},
                            {"ACBDE",
                            changeIntToString(duration),
                            changeIntToString(distance),
                            changeFloatToString(traffic)});
}
/* *It is exactly the same as ROUTE_2 conseqently in the same direction
void GpsAgent::calculatePropertiesForRouteACDEC()
{
    int duration = calculateDuration({2, 8, 6, 7});
    int distance = calculateDistance({2, 8, 6, 7});
    float traffic = calculateTraffic({2, 8, 6, 7});
    routeMos_.push_back(wrapper_->createObject("CAR/GPS/ROUTES/ROUTE_3", "Calculated", "Enabled"));
    wrapper_->expandObject(&(routeMos_.back()), {"type", "duration", "distance", "traffic"}, {"ACBDE", duration.c_str(), distance.c_str(), traffic.c_str()});
}
*/

void GpsAgent::calculatePropertiesForRouteADBCE()
{
    int duration = calculateDuration({3, 6, 5, 9});
    int distance = calculateDistance({3, 6, 5, 9});
    float traffic = calculateTraffic({3, 6, 5, 9});
    routeMos_.push_back(wrapper_->createObject("CAR/GPS/ROUTES/ROUTE_5", "Calculated", "Enabled"));
    wrapper_->expandObject(&(routeMos_.back()),
                            {"type", "duration", "distance", "traffic"},
                            {"ACBDE",
                            changeIntToString(duration),
                            changeIntToString(distance),
                            changeFloatToString(traffic)});
}

void GpsAgent::calculatePropertiesForRouteABECD()
{
    int duration = calculateDuration({1, 7, 9, 8});
    int distance = calculateDistance({1, 7, 9, 8});
    float traffic = calculateTraffic({1, 7, 9, 8});
    routeMos_.push_back(wrapper_->createObject("CAR/GPS/ROUTES/ROUTE_6", "Calculated", "Enabled"));
    wrapper_->expandObject(&(routeMos_.back()),
                            {"type", "duration", "distance", "traffic"},
                            {"ACBDE",
                            changeIntToString(duration),
                            changeIntToString(distance),
                            changeFloatToString(traffic)});
}
void GpsAgent::calculatePropertiesForRouteAEBCD()
{
    int duration = calculateDuration({4, 9, 5, 8});
    int distance = calculateDistance({4, 9, 5, 8});
    float traffic = calculateTraffic({4, 9, 5, 8});

    routeMos_.push_back(wrapper_->createObject("CAR/GPS/ROUTES/ROUTE_7", "Calculated", "Enabled"));
    wrapper_->expandObject(&(routeMos_.back()),
                            {"type", "duration", "distance", "traffic"},
                            {"ACBDE",
                            changeIntToString(duration),
                            changeIntToString(distance),
                            changeFloatToString(traffic)});
}

int GpsAgent::calculateDuration(vector<int> sequence)
{
    int duration = 0;
    for(const auto &element: sequence)
    {
        duration += changeStringToNumber(targetMos_[element - 1].details[2].second);
    }
    Logger::saveToFile("CARM/DBG: GpsAgent: calculateDuration " + changeIntToString(duration));
    return duration;
}

int GpsAgent::calculateDistance(vector<int> sequence)
{
    int distance;
    for(const auto &element: sequence)
    {
        distance += changeStringToNumber(targetMos_[element - 1].details[3].second);
    }
    Logger::saveToFile("CARM/DBG: GpsAgent: calculateDistance " + changeIntToString(distance));
    return distance;
}

float GpsAgent::calculateTraffic(vector<int> sequence)
{
    float traffic = 0;
    int i = 0;
    for( const auto &element : sequence)
    {
        traffic += transformTrafficConvestion(targetMos_[element - 1].details[4].second);
        i++;
    }
    return traffic / static_cast<float>(i);

}

float GpsAgent::transformTrafficConvestion(string convestionType)
{
    if(convestionType == "Mild")
    {
        return 1.0;
    }
    else if(convestionType == "Medium")
    {
        return 1.2;
    }
    else if(convestionType == "Heavy")
    {
        return 1.5;
    }
    else
    {
        return 1.0;
    }
}

int GpsAgent::getConstrains()
{
    string type;
    string value;
    Mo constrainMo = wrapper_->getObject("CAR/GPS/CONSTRAIN");
    if(constrainMo.distname == "CAR/GPS/CONSTRAIN")
    {
        type = wrapper_->getDetailValue(constrainMo.distname, "type");
        value = wrapper_->getDetailValue(constrainMo.distname, "value");
        Logger::saveToFile("CAR/INF: GpsAgent: getConstrain: Constrain is available for " + type + " and is equal " + value);
    }
    else
    {
        Logger::saveToFile("CAR/WRN: GpsAgent: getConstain: Constrains are unavailable");
        return 0;
    }
    return changeStringToFloat(value);
}

float GpsAgent::calculateNecessaryFuel()
{
    string carAvgFuelCons = wrapper_->getDetailValue("CAR", "avgFuelConsumption");
    float avgFuelConsuption = changeStringToFloat(carAvgFuelCons);
    for(auto &route : routeMos_)
    {
        float distance = changeStringToFloat(route.details[2].second);
        float traffic = changeStringToFloat(route.details[3].second);
        float consumedFuel = distance * avgFuelConsuption * traffic / 100;
        wrapper_->expandObject(&route, {"fuelCost"}, {changeFloatToString(consumedFuel)});
    }
}

void GpsAgent::selectRoutesWithConstrains()
{
    float constrain = getConstrains();
    std::cout << "Constrain: " << constrain <<std::endl;
    if (constrain != 0)
    {
        for(vector<Mo>::iterator i = routeMos_.begin(); i < routeMos_.end() + 1; i++)
        {
            if(changeStringToFloat((*i).details[4].second) > constrain)
            {
                Logger::saveToFile(("CARM/INF: GpsAgent: object will be deleted: " + (*i).distname).c_str());
                wrapper_->deleteObject((*i).distname);
                routeMos_.erase(i);
            }
        }
    }

}

Mo GpsAgent::findRouteWithTheShortestDistance()
{
    Logger::saveToFile("CARM/BDG: GpsAgent: findTheShortestDistance");
    Mo route = routeMos_.front();
    for(vector<Mo>::iterator i= routeMos_.begin() + 1 ; i < routeMos_.end() ; i++)
    {
        if(changeStringToFloat((*i).details[2].second) < changeStringToFloat((*(i-1)).details[2].second) &&
            changeStringToFloat((*i).details[2].second) <= changeStringToFloat(route.details[2].second))
        {
            Logger::saveToFile("CARM/DBG: GpsAgent: Distance: route is set to some value");
            route = (*i);
        }
    }
    return route;
}

Mo GpsAgent::findRouteWithTheShortestDuration()
{
    Logger::saveToFile("CARM/BDG: GpsAgent: findTheShortestDuration");
    Mo route = routeMos_.front();
    for(vector<Mo>::iterator i= routeMos_.begin() + 1 ; i < routeMos_.end() -1 ; i++)
    {
        Logger::saveToFile("CARM/DBG: GpsAgent: findTheShortestDuration equaling...");
        if(changeStringToFloat((*i).details[1].second) < changeStringToFloat((*(i-1)).details[1].second) &&
            changeStringToFloat((*i).details[1].second) <= changeStringToFloat(route.details[1].second))
        {
            Logger::saveToFile("CARM/DBG: GpsAgent: Duration: route is set to some value");
            route = (*i);
        }
    }
    return route;
}

Mo GpsAgent::findRouteWithTheSmallestFuelConsumption()
{
    Logger::saveToFile("CARM/BDG: GpsAgent: findTheSmallestFuelConsumption");
    Mo route = routeMos_.front();
    for(vector<Mo>::iterator i= routeMos_.begin() + 1 ; i < routeMos_.end() ; i++)
    {
        if(changeStringToFloat((*i).details[4].second) < changeStringToFloat((*(i-1)).details[4].second) &&
            changeStringToFloat((*i).details[4].second) < changeStringToFloat(route.details[4].second))
        {
            Logger::saveToFile("CARM/DBG: GpsAgent: FuelCons: route is set to some value");
            route = (*i);
        }
    }
    return route;
}

Mo GpsAgent::calculateDecision()
{
    Logger::saveToFile("CARM/DBG: GpsAgent: calculateDecision");
    float factors[3];
    Mo theShortestDuration = findRouteWithTheShortestDuration();
    Mo theShortestDistance = findRouteWithTheShortestDistance();
    Mo theSmallesFuelConsumption = findRouteWithTheSmallestFuelConsumption();
    factors[0] = ( 0.3 * changeStringToFloat(theShortestDuration.details[1].second) +
                           0.3 * changeStringToFloat(theShortestDuration.details[2].second) +
                           0.3 * changeStringToFloat(theShortestDuration.details[4].second));
    factors[1] = ( 0.3 * changeStringToFloat(theShortestDistance.details[1].second) +
                       0.3 * changeStringToFloat(theShortestDistance.details[2].second) +
                       0.3 * changeStringToFloat(theShortestDistance.details[4].second));
    factors[2] = ( 0.3 * changeStringToFloat(theSmallesFuelConsumption.details[1].second) +
                       0.3 * changeStringToFloat(theSmallesFuelConsumption.details[2].second) +
                       0.3 * changeStringToFloat(theSmallesFuelConsumption.details[4].second));
    float minimum = factors[0];
    for(int i = 0; i < 3; i++)
    {
        if(factors[i] <= minimum)
        {
            minimum = factors[i];
        }

    }
    if(factors[0] == minimum)
    {
        return theShortestDuration;
    }
    else if(factors[1] == minimum)
    {
        return theShortestDistance;
    }
    else if(factors[2] == minimum)
    {
        return theSmallesFuelConsumption;
    }
    else
    {
        Logger::saveToFile("CARM/WRN: GpsAgent: Solution cannot be found");
    }
}


Mo GpsAgent::calculateParetoDominium()
{
    Mo route = findRouteWithTheShortestDistance();
    for(vector<Mo>::iterator i= routeMos_.begin() + 1; i < routeMos_.end(); i++)
    {
        Logger::saveToFile("CARM/DBG: GpsAgent: findTheShortestDuration equaling...");
        if(changeStringToFloat((*i).details[1].second) < changeStringToFloat((*(i-1)).details[1].second) &&
            changeStringToFloat((*i).details[1].second) <= changeStringToFloat(route.details[1].second))
        {
            if(changeStringToFloat((*i).details[2].second) <= changeStringToFloat(route.details[2].second) &&
               changeStringToFloat((*i).details[4].second) <= changeStringToFloat(route.details[4].second))
            {
               Logger::saveToFile("CARM/DBG: GpsAgent: Duration: route is set to some value");
               route = (*i);
            }

        }
    }
    return route;
}

Mo GpsAgent::calculateDecisionAndMinimizeConstrainedParam()
{
    Mo route = routeMos_.at(0);
    std::cout << "Size? " << routeMos_.size() << std::endl;
    for (vector<Mo>::iterator i = routeMos_.begin(); i <routeMos_.end(); i++)
    {
        std::cout << "Route: " << (*i).distname << " has " << (*i).details[4].second << std::endl;

        if (changeStringToFloat((*i).details[4].second) <= changeStringToFloat(route.details[4].second))
        {
            route = (*i);
        }
    }
    std::cout << "With paramtrized: " << route.distname << std::endl;
    return route;
}

bool GpsAgent::isTargetLine(string line)
{
    if(line.find("Target") != string::npos)
    {
        return true;
    }
    return false;
}

bool GpsAgent::verifyResultFile(fstream* file)
{
    if( file->good() == true )
    {
        Logger::saveToFile("CARM/INF: GpsAgent: createRoutesTopology: file exists");
        return true;

    }
    else
    {
        Logger::saveToFile("CARM/ERR: GpsAgent: createRoutesTopology: file doesnt exist");
        return false;
    }
}

void GpsAgent::getRoutes(string destination)
{
    sendGoogleMapsRequestForDestination(destination);
}

string GpsAgent::getLocation()
{
    //cout << runExternalScripts("D:/private/IDB/HWEMU/GeoLocationAPI.py", "D:/private/IDB/SYS/TEMP/location.txt", {})<<endl;
    return runExternalScripts("D:/private/IDB/HWEMU/GeoLocationAPI.py", "D:/private/IDB/SYS/TEMP/location.txt", {});
}

void GpsAgent::sendGoogleMapsRequestForDestination(string destination)
{
    string data = runExternalScripts("D:/private/IDB/HWEMU/BMAPI/GetRoutes.py", "D:/private/IDB/SYS/TEMP/routes.txt", {gpsMo_.details[0].second, gpsMo_.details[1].second, destination});
    //setRoutes(data);
}

string GpsAgent::runExternalScripts(string path, string resultsFilePath, vector<string> args)
{
    string operation = "python " + path + " ";
    string endOperation = " >> " + resultsFilePath;
    string executable = operation;
    for(const auto &arg : args)
    {
       executable += arg + " ";
    }
    executable += endOperation;
    //cout << executable <<endl;
    system(executable.c_str());
    fstream plik( resultsFilePath, std::ios::in );
    char bufor[ 1024 ];
    plik.read( bufor, 1024 );
    plik.close();
    remove("D:/private/IDB/SYS/TEMP/routes.txt");
    return string(bufor);
}

vector<string> GpsAgent::splitString(string strPtr, char sign)
{

    for (int i=0; i < strPtr.length(); i++)
    {
        if (strPtr[i] == ' ')
            strPtr.erase(i, 1);
        if (strPtr[i] == sign)
            strPtr[i] = ' ';

    }
    vector<string> splitted;
    stringstream ss(strPtr);
    string temp;
    while (ss >> temp)
    {
        splitted.push_back(temp);
    }
    return splitted;
}

string GpsAgent::changeIntToString(int i)
{
    stringstream ss;
    string temp;
    ss << i;
    ss >> temp;
    return temp;

}

string GpsAgent::changeFloatToString(float i)
{
    stringstream ss;
    string temp;
    ss << i;
    ss >> temp;
    return temp;
}

int GpsAgent::changeStringToNumber(string i)
{
    stringstream ss(i);
    int temp;
    ss >> temp;
    return temp;

}

float GpsAgent::changeStringToFloat(string i)
{
    stringstream ss(i);
    float temp;
    ss >> temp;
    return temp;
}




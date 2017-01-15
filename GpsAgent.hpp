#ifndef GPSMANAGER_H
#define GPSMANAGER_H
#include "Client.hpp"
#include <istream>
#include <ostream>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "Logger.hpp"
#include "IDBWrapper.hpp"
#include "DSSAlgorithm.hpp"


using namespace std;

class GpsAgent
{
    public:
        GpsAgent(IDBWrapper* wrapper);
        virtual ~GpsAgent();
        bool createGpsMo();
        void setCarPosition();
        void getRoutes(string destination);
        string getLocation();
        void waitForNavigateRequest();


    protected:

    private:
        vector<string> splitString(string strPtr, char sign);

        Mo routeMo_;
        vector<vector<string>> route_;
        Mo targetsMo_;
        vector<Mo> targetMos_;
        vector<Mo> routeMos_;
        Mo routesMo_;
        Client* client_;
        IDBWrapper* wrapper_;
        DSSAlgorithm* dss_;
        Mo gpsMo_;
        void setRoutes(string rawData);
        void sendGoogleMapsRequestForDestination(string destination);
        void createTargetsTopology(Mo navigateReqMo);
        void createRoutesTopology();
        void setResult(string distname);
        Mo findRouteWithTheShortestDistance();
        Mo findRouteWithTheShortestDuration();
        Mo findRouteWithTheSmallestFuelConsumption();
        Mo calculateDecision();
        Mo calculateParetoDominium();
        Mo calculateDecisionAndMinimizeConstrainedParam();
        void addParetoDetailValueForObject(string distname);
        void addParamterizedDetailValueForObject(string distname);
        void calculatePropertiesForRouteABCDE();
        void calculatePropertiesForRouteACBDE();
        void calculatePropertiesForRouteACDBE();
        void calculatePropertiesForRouteABDCE();
        void calculatePropertiesForRouteADBCE();
        void calculatePropertiesForRouteABECD();
        void calculatePropertiesForRouteAEBCD();
        void calculateProperties();
        int calculateDuration(vector<int> sequence);
        int calculateDistance(vector<int> sequence);
        float calculateTraffic(vector<int> sequence);
        float transformTrafficConvestion(string convestionType);
        int getConstrains();
        float calculateNecessaryFuel();
        void selectRoutesWithConstrains();
        bool verifyResultFile(fstream* file);
        bool isTargetLine(string line);
        void getConstrain();
        void calculate(string constrainType);
        string changeIntToString(int i);
        string changeFloatToString(float i);
        int changeStringToNumber(string i);
        float changeStringToFloat(string i);
        string runExternalScripts(string path, string resultsFilePath, vector<string> args);



};

#endif // GPSMANAGER_H

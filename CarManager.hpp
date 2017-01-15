#ifndef CARMANAGER_H
#define CARMANAGER_H
#include "Client.hpp"
#include "GpsAgent.hpp"
#include <iostream>
#include <vector>
#include "Logger.hpp"
#include "IDBWrapper.hpp"
#include "DriveController.hpp"
using namespace std;

class CarManager
{
    public:

        CarManager(IDBWrapper* wrapper);
        virtual ~CarManager();
        void createCARObject();
        void setOperationalState(string value);
        void setProceduralState(string value);
        void createGPSObject();
        void setGpsPosition();
        void createFARObject();
        void subscribeForSystemReady();
        void setReadyToUse();
        void navigate();
        void checkNavigationRequest();
        void checkDriveTypeRequest(){Logger::saveToFile("checkDirveTypeRequest");driveMan_->checkForDriveTypeChangeRequest();}
    protected:

    private:
        Client* client_;
        IDBWrapper* wrapper_;
        GpsAgent* gpsM_;
        DriveController* driveMan_;
        Mo carMo_;
        Mo bdmMo_;
        Mo idmMo_;
        Mo gpsMo_;

        void setObserverOnBdmObjectOnConfiguredValue();
        void setObserverOnIdmObjectOnConifguredValue();

};

#endif // CARMANAGER_H

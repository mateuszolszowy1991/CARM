#include "CarManager.hpp"

CarManager::CarManager(IDBWrapper* wrapper) : wrapper_(wrapper),
                                         gpsM_(new GpsAgent(wrapper_)),
                                         driveMan_(new DriveController(wrapper_))
{
    Logger::saveToFile("CarManager/INF: Ctor");
}

CarManager::~CarManager()
{
    //dtor
}

void CarManager::createCARObject()
{
    Logger::saveToFile("CARM/INF: CarManager: createCARObject" );
    carMo_ = wrapper_->createObject("CAR", "Online", "Initializing");
    vector<string> modelProperty {"Model", "avgFuelConsumption"};
    vector<string> modelValue {"RenaultMegane", "8.0"};
    wrapper_->expandObject(&carMo_, modelProperty, modelValue);
}

void CarManager::setOperationalState(string value)
{
    Logger::saveToFile("CARM/INF: CarManager: setOperationalState to value: " + value);
    wrapper_->setOperationalState(&carMo_, value);

}

void CarManager::setProceduralState(string value)
{
    Logger::saveToFile("CARM/INF: CarManager: setProceduralState to value: " + value);
    wrapper_->setProceduralState(&carMo_, value);
    Logger::saveToFile("SET " + carMo_.operationalState);
}

void CarManager::setObserverOnBdmObjectOnConfiguredValue()
{
    Logger::saveToFile("CARM/INF: CarManager: setObserverOnBdmObjectOnConfiguredValue ");
    bdmMo_ = wrapper_->setObserverForObjectCreate("CAR/BDM");
    wrapper_->setObserverForObjectPropertiesValue(&bdmMo_, bdmMo_.proceduralStatePtr, "Configured");
    Logger::saveToFile("CARM/INF: CarManager: setObserverOnBdmObjectOnConfiguredValue: BDM is configured ");
}

void CarManager::setObserverOnIdmObjectOnConifguredValue()
{
    Logger::saveToFile("CARM/INF: CarManager: setObserverOnIdmObjectOnConfiguredValue ");
    idmMo_ = wrapper_->setObserverForObjectCreate("CAR/IDM");
    wrapper_->setObserverForObjectPropertiesValue(&idmMo_, idmMo_.proceduralStatePtr, "Configured");
    Logger::saveToFile("CARM/INF: CarManager: setObserverOnIdmObjectOnConfiguredValue: IDM is configured ");
}

void CarManager::createGPSObject()
{
   if(gpsM_->createGpsMo())
   {

       //gpsM_->setCarPosition();
       //gpsM_->sendGoogleMapsRequestForDestination("Berlin");
   }
}

void CarManager::checkNavigationRequest()
{
    Logger::saveToFile("CheckNavigationRequest");
   gpsM_->waitForNavigateRequest();
}

void CarManager::setGpsPosition()
{

}

void CarManager::createFARObject()
{
    wrapper_->createObject("CAR/FAR", "Online", "Enabled");
}

void CarManager::subscribeForSystemReady()
{
    setObserverOnBdmObjectOnConfiguredValue();
    setObserverOnIdmObjectOnConifguredValue();
}

void CarManager::setReadyToUse()
{
    wrapper_->setProceduralState(&carMo_, "Ready_to_use");
    Mo doorsMo = wrapper_->getObject("CAR/BDM/DOORS");
    wrapper_->setObserverForObjectPropertiesValue(&doorsMo, doorsMo.proceduralStatePtr, "Unlocked");
    Logger::saveToFile("CARM/INF: User action detected");
}

void CarManager::navigate()
{
    string target;
    cout << "Put target: ";
    cin >> target;
    gpsM_->getRoutes(target);
    cout << "Put target: ";
    cin >> target;
    gpsM_->getRoutes(target);
    cout << "Put target: ";
    cin >> target;
    gpsM_->getRoutes(target);
}


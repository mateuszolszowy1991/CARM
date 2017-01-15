#include "Logger.hpp"
#include "Client.hpp"
#include "CarManager.hpp"

#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argc , char *argv[])
{

    Logger::clearLogs();
    Logger::saveToFile("CarManager application started ");
    Client* client = new Client();

    if(client->clientInitialize())
    {
        client->connectToIDB();
        client->start();
    }
    IDBWrapper* wrapper = new IDBWrapper(client, "CARM");
    wrapper->welcomeMessage();

    //PlanExecutor* planExec = new PlanExecutor();
    //planExec->validateMetaVersion();
    CarManager* cM = new CarManager(wrapper);
    cM->createCARObject();
    cM->createGPSObject();

    cM->createFARObject();
    //cM->subscribeForSystemReady();
    //cM->setReadyToUse();
    //cM->navigate();
    while(true)
    {
        cM->checkNavigationRequest();
        cM->checkDriveTypeRequest();
        sleep(1);
    }





    return 0;
}

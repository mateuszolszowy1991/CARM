#ifndef IDBWRAPPER_H
#define IDBWRAPPER_H
#include "Logger.hpp"
#include <vector>
#include <sstream>
#include "../Client.hpp"
using namespace std;

struct Mo
        {
            string distname;
            string operationalStatePtr;
            string proceduralStatePtr;
            string operationalState;
            string proceduralState;
            vector<pair<string, string>> details;
            void show(){cout << "distname: " << distname << " operStPtr: " << operationalStatePtr << " procStPtr: " << proceduralStatePtr << " operState: "
                        << operationalState << " procState: " <<proceduralState <<endl;}
            void showDetails()
            {
                for(const auto &detail : details)
                {
                    Logger::saveToFile("Mo: showDetail: " + detail.first + ": " + detail.second);
                }

            }
        };

class IDBWrapper
{
    public:
        void welcomeMessage();
        Mo createObject(string distname, string operationalState, string proceduralState);
        void setOperationalState(Mo* mo, string value);
        void setProceduralState(Mo* mo, string value);
        Mo setObserverForObjectCreate(string distname);
        Mo observeAndDo(string distname, void (*action)(Mo mo));
        void setObserverForObjectPropertiesValue(Mo* mo, string propertyPtr, string value);
        void expandObject(Mo* mo, vector<string> propertiesName, vector<string> values);
        string getDetailValue(string distname, string detailName);
        void getDetailValue(Mo* mo, string detailName);
        Mo getObject(string distname);
        string deleteObject(string distname);
        IDBWrapper(Client* client, string binaryName);
        ~IDBWrapper();
    protected:

    private:
        Client* client_;
        string binaryName_;
        void showAvailableRequests();
        vector<pair<string, string>> availableRequests_;
        vector<string> splitString(string strPtr, char sign);
};

#endif // IDBWRAPPER_H

#include "IDBWrapper.hpp"

IDBWrapper::IDBWrapper(Client* client, string binaryName) : client_(client), binaryName_(binaryName)
{
    //ctor
}

IDBWrapper::~IDBWrapper()
{
    //dtor
}

void IDBWrapper::welcomeMessage()
{

    string response=client_->sendRequest("0x0-CARM-Welcome");
    vector<string> splittedResponse = splitString(response, '-');
    vector<string> availableRequests = splitString(splittedResponse[3], ';');
    Logger::saveToFile("go into LOOP");
    for(const auto &line : availableRequests)
    {
        if(line != "welcome")
        {
            Logger::saveToFile("Line: " + line);
            vector<string> request = splitString(line, ':');
            availableRequests_.push_back(pair<string, string>(request[0], request[1]));
        }
    }
    showAvailableRequests();
}

void IDBWrapper::showAvailableRequests()
{
    Logger::saveToFile("Available Requests:");
    for(const auto &request : availableRequests_)
    {

        Logger::saveToFile(request.first + ": " + request.second);
    }
}
Mo IDBWrapper::createObject(string distname, string operationalState, string proceduralState)
{
    //Logger::saveToFile("CARM/INF: CarManager: createCARObject" );
    Mo mo;
    string response = client_->sendRequest(("0x1-CARM-"+distname).c_str());
    Logger::saveToFile("CARM/INF: CarManager: recevied response: "+ response );
    vector<string> splittedMsg = splitString(response, '-');
    vector<string> splittedResponse = splitString(splittedMsg[2], ';');
    splittedMsg.clear();
    mo.distname = splittedResponse[0];
    mo.operationalStatePtr = splittedResponse[1];
    mo.proceduralStatePtr = splittedResponse[2];
    setOperationalState(&mo, operationalState);
    mo.operationalState = operationalState;
    setProceduralState(&mo, proceduralState);
    mo.proceduralState = proceduralState;
    return mo;
}

void IDBWrapper::setOperationalState(Mo* mo, string value)
{
    string response = client_->sendRequest(("0x3-CARM-"+mo->distname+";"+mo->operationalStatePtr + ";" + value).c_str());
    if(response.size())
    {
        mo->operationalState = value;
    }
}

void IDBWrapper::setProceduralState(Mo* mo, string value)
{
    string response = client_->sendRequest(("0x3-CARM-"+mo->distname+";"+mo->proceduralStatePtr + ";" + value).c_str());
    if(response.size())
    {
        mo->proceduralState = value;
    }
}

Mo IDBWrapper::setObserverForObjectCreate(string distname)
{
    Mo mo;
    vector<string> splittedResponse;
    string msg = "0x5-" + binaryName_ + "-" + distname;
    string response = client_->sendRequest(msg.c_str());
    size_t found = response.find("#");

    if (found != string::npos)
    {
        splittedResponse = splitString(client_->receive(), '-');
        splittedResponse = splitString(splittedResponse[2], ';');
        mo.distname = splittedResponse[0];
        mo.proceduralStatePtr = splittedResponse[1];
        mo.operationalStatePtr = splittedResponse[2];
    }
    else
    {
        response = client_->receive();
        if(response.size())
        {
            splittedResponse = splitString(client_->receive(), '-');
            splittedResponse = splitString(splittedResponse[2], ';');
            mo.distname = splittedResponse[0];
            mo.proceduralStatePtr = splittedResponse[1];
            mo.operationalStatePtr = splittedResponse[2];
        }
    }
    //cout << mo.distname <<endl;
    return mo;

}

Mo IDBWrapper::observeAndDo(string distname, void (*action)(Mo mo))
{
    Mo mo = setObserverForObjectCreate(distname);
    action(mo);
}

void IDBWrapper::setObserverForObjectPropertiesValue(Mo* mo, string propertyPtr, string value)
{
    string response = client_->sendRequest(("0x5-" + binaryName_ + "-"+mo->distname+";"+propertyPtr+";"+value).c_str());

    if(response.find("#"))
    {
        vector<string> splittedResponse = splitString(response, '-');
        if(propertyPtr == mo->operationalStatePtr)
        {
            mo->operationalState = splittedResponse[2];
        }
        else
        {
            mo->proceduralState = splittedResponse[2];
        }
    }
    else
    {
        response = client_->receive();
        if(response.size())
        {
           vector<string> splittedResponse = splitString(response, '-');
            if(propertyPtr == mo->operationalStatePtr)
            {
                mo->operationalState = splittedResponse[2];
            }
            else
            {
                mo->proceduralState = splittedResponse[2];
            }
        }

    }
}

void IDBWrapper::expandObject(Mo* mo, vector<string> propertiesName, vector<string> values)
{
    int i = 0;
    string response;
    for(const auto &propertyName: propertiesName)
    {
        response = client_->sendRequest(("0x6-" + binaryName_ + "-"+mo->distname + ";" + propertyName + ";" + values[i]).c_str());
        if(response.size())
        {
            mo->details.push_back(make_pair(propertyName, values[i]));
        }
        i++;
    }
    mo->showDetails();

}

Mo IDBWrapper::getObject(string distname)
{
    string response = client_->sendRequest(("0x8-" + binaryName_ + "-" + distname).c_str());
    vector<string> splittedResponse = splitString(response, '-');
    if(splittedResponse[2] != "0")
    {
        vector<string> ptrs = splitString(splittedResponse[2], ';');
        Mo mo = {ptrs[0], ptrs[1], ptrs[2]};
        return mo;
    }
    Mo mo = {"", "", ""};
    return mo;

}

string IDBWrapper::getDetailValue(string distname, string detailName)
{
    string response = client_->sendRequest(("0x9-"+ binaryName_ + "-" + distname + ";" + detailName).c_str());
    vector<string> ptrs = splitString(response, '-');
    return ptrs[2];
}


void IDBWrapper::getDetailValue(Mo* mo, string detailName)
{
    string response = client_->sendRequest(("0x9-"+ binaryName_ + "-" + mo->distname + ";" + detailName).c_str());
    vector<string> ptrs = splitString(response, '-');
    pair<string, string> detail = make_pair(detailName, ptrs[2]);
    mo->details.push_back(detail);
}

string IDBWrapper::deleteObject(string distname)
{
    string response = client_->sendRequest(("0x4-"+ binaryName_ + "-" + distname).c_str());
    return response;
}

vector<string> IDBWrapper::splitString(string strPtr, char sign)
{
    for (int i=0; i < strPtr.length(); i++)
    {
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

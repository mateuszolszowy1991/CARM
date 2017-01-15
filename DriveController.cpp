#include "DriveController.hpp"

DriveController::DriveController(IDBWrapper* wrapper) : wrapper_(wrapper)
{
    Logger::saveToFile("CAR/INF: CarManager: DriveController ctor");
}

DriveController::~DriveController()
{
    //dtor
}

void DriveController::checkForDriveTypeChangeRequest()
{
    Mo mo = wrapper_->getObject("CAR/DRIVE_TYPE_CHANGE_REQ");
    if(mo.distname == "CAR/DRIVE_TYPE_CHANGE_REQ")
    {
        wrapper_->getDetailValue(&mo, "type");
        changeDriveType(mo.details[0].second);
    }
}

void DriveController::changeDriveType(string value)
{
    Logger::saveToFile("CARM/DriveController: checkForDriveTypeChangeRequest handled | value: " + value);
    wrapper_->deleteObject("CAR/DRIVE_TYPE_CHANGE_REQ");
}


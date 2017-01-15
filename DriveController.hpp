#ifndef DRIVECONTROLLER_H
#define DRIVECONTROLLER_H
#include "Logger.hpp"
#include "IDBWrapper.hpp"

class DriveController
{
    public:
        DriveController(IDBWrapper* wrapper);
        virtual ~DriveController();
        void checkForDriveTypeChangeRequest();

    protected:

    private:
        void changeDriveType(string value);
        IDBWrapper* wrapper_;
};

#endif // DRIVECONTROLLER_H

#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <time.h>
#define DTTMFMT "%H:%M:%S "
#define DTTMSZ 15

class Logger
{
    public:
       static void saveToFile(std::string data);
       static void clearLogs();
    protected:

    private:
        static char* getDtTm ();

};

#endif // DUMPDBTOFILE_H

#include "Logger.hpp"

void Logger::saveToFile(std::string napis)
{
    std::fstream plik( "/home/mato3/OSCAR/SYS/TEMP/CARM_LOGS.txt", std::ios::app );
    //std::string date = static_cast<std::string>(getDtTm());
    std::string log =  napis + "\n";
    plik.write( & log[ 0 ], log.length() );
    plik.close();
}

void Logger::clearLogs()
{
    std::fstream plik( "/home/mato3/OSCAR/SYS/TEMP/CARM_LOGS.txt", std::ios::out );
    plik.write( "", 0 );
    plik.close();
}

char* Logger::getDtTm()
{   char buff[DTTMSZ];
    time_t t = time (0);
    strftime (buff, DTTMSZ, DTTMFMT, localtime (&t));
    return buff;
}

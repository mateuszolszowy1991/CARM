#ifndef CLIENT_H
#define CLIENT_H
#include <stdio.h>
#include <string.h>	//strlen
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <iostream>
#include "Logger.hpp"

using namespace std;

class Client
{
    public:
        Client();
        virtual ~Client();
        int clientInitialize();
        void connectToIDB();
        char* sendRequest(const char* message);
        void start();
        char* receive();
    protected:

    private:
        int sock;
        struct sockaddr_in server;
        char message[1000] , server_reply[2000];
};

#endif // CLIENT_H

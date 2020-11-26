// #define WIN32_LEAN_AND_MEAN
// #define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <thread>
#ifdef _WIN32
    #include <Windows.h>
    #include <WinSock2.h>
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <string.h>

    #define SOCKET int
    #define INVALID_SOCKET	(SOCKET)(~0)
    #define SOCKET_ERROR	(-1)
#endif

class dataHeader
{
public:
    int dataLen;
    int cmd;
};
enum CMD {
    LOG_IN,
    LOG_OUT
};
class login : public dataHeader
{
public:
    login()
    {
        dataLen = sizeof(login);
        cmd = LOG_IN;
    }
    char user_name[32];
    char passwd[32];
};

struct response : public dataHeader
{
public:
    response()
    {
        dataLen = sizeof(response);
        cmd = LOG_IN;
        ret = 0;
    }
    int ret;
    char text[128];
};

int send_server(SOCKET c_sock);
int recv_server(SOCKET c_sock);
void cin_cmd(SOCKET c_sock);
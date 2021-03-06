#include "easy_tcp_client.h"

EasyTcpClient::EasyTcpClient() { m_sock = INVALID_SOCKET; }

EasyTcpClient::~EasyTcpClient() { Close(); }

bool EasyTcpClient::initSocket()
{
#ifdef _WIN32
    // windows启动socket
    WORD ver = MAKEWORD(2, 2);
    WSADATA dat;
    WSAStartup(ver, &dat);
#endif
    if (m_sock != INVALID_SOCKET)
    {
        std::cout << "close last connect socket<" << m_sock << ">." << std::endl;
        Close();
    }
    // 1.建立socket
    m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (m_sock == INVALID_SOCKET)
    {
        std::cout << "create socket fail!" << std::endl;
        return false;
    }
    return true;
}

bool EasyTcpClient::Connect(char *ip, short int port)
{
    // 2.connect server
    sockaddr_in _sin = {};
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(port);
#ifdef _WIN32
    _sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
    _sin.sin_addr.s_addr = inet_addr(ip);
#endif
    if (m_sock == INVALID_SOCKET)
    {
        initSocket();
    }

    int ret = connect(m_sock, (sockaddr *)&_sin, sizeof(_sin));

    if (INVALID_SOCKET == ret)
    {
        std::cout << "socket<" << m_sock << ">connect fail!" << std::endl;
        m_sock = INVALID_SOCKET;
    }

    return true;
}

void EasyTcpClient::Close()
{
    if (m_sock != INVALID_SOCKET)
    {
#ifdef _WIN32
        closesocket(m_sock);
        WSACleanup();
#else
        close(m_sock);
#endif
    }
    m_sock = INVALID_SOCKET;
}

bool EasyTcpClient::isRun() { return m_sock != INVALID_SOCKET; }

bool EasyTcpClient::onRun()
{
    if (isRun())
    {
        fd_set fdRead;

        FD_ZERO(&fdRead);
        FD_SET(m_sock, &fdRead);
        timeval tTime = {0, 0};
        int ret = select(m_sock + 1, &fdRead, nullptr, nullptr, &tTime);
        // int ret = select(m_sock + 1, &fdRead, nullptr, nullptr, nullptr);
        if (ret < 0)
        {
            std::cout << "select exit!" << std::endl;
            return false;
        }
        if (FD_ISSET(m_sock, &fdRead))
        {
            FD_CLR(m_sock, &fdRead);
            if (-1 == recvData())
            {
                m_sock = INVALID_SOCKET;
                return false;
            }
        }
        return true;
    }
    return false;
}

int EasyTcpClient::recvData()
{
    int nLen = recv(m_sock, m_szRecv, RECV_MAX_SIZE, 0);
    if (nLen <= 0)
    {
        std::cout << "client close" << std::endl;
        return -1;
    }
    memcpy(m_szMsg + m_lastPos, m_szRecv, nLen);
    m_lastPos += nLen;
    while (m_lastPos >= sizeof(dataHeader))
    {
        dataHeader *header = (dataHeader *)m_szMsg;
        if (m_lastPos >= header->dataLen)
        {
            int leftLen = m_lastPos - header->dataLen;
            onNetMsg(header);
            memcpy(m_szMsg, m_szMsg + header->dataLen, leftLen);
            m_lastPos = leftLen;
        }
        else
        {
            break;
        }
    }
    return 0;
}

int EasyTcpClient::onNetMsg(dataHeader *header)
{
    switch (header->cmd)
    {
    case LOG_IN_RESPONSE:
    {
        loginResponse *rsp = (loginResponse *)header;

        // std::cout << "recv data len:" << header->dataLen << std::endl;
    }
    break;
    case LOG_OUT_RESPONSE:
    {
        logOutResponse *rsp = (logOutResponse *)header;

        std::cout << "recv data len:" << header->dataLen << std::endl;
    }
    break;
    case NEW_USER_JOIN:
    {
        newUserJoin *rsp = (newUserJoin *)header;
        std::cout << "recv data len:" << header->dataLen << std::endl;
    }
    break;
    default:
        std::cout << "error cmd!" << std::endl;
        break;
    }
    return 0;
}

int EasyTcpClient::sendData(dataHeader *header)
{
    if (isRun())
    {
        int ret = send(m_sock, (char *)header, header->dataLen, 0);
        if(ret == SOCKET_ERROR)
        {
            m_sock = INVALID_SOCKET;
            return SOCKET_ERROR;
        }
        return ret;
    }
    else
    {
        std::cout << "socket close!" << std::endl;
        m_sock = INVALID_SOCKET;
        return 0;
    }
}
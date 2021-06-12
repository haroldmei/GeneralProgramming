/*
 * This source file is part of the ace-radius library.  This code was
 * written by Alex Agranov in 2004-2009, and is covered by the BSD open source
 * license. Refer to the accompanying documentation for details on usage and
 * license.
 */

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <sys/types.h>
#ifdef _MSC_VER
    #include <winsock2.h>
#else
    #include <unistd.h>
    #include <sys/socket.h>
#endif
#if HAVE_SYS_IOCTL_H
    #include <sys/ioctl.h>
#endif
#if HAVE_SYS_FILIO_H
    #include <sys/filio.h>
#endif        

#include "RadiusClientStack.h"
#include "RadiusClientConnection.h"
#include "md5.h"

// ---------------------------
// Constructor
// ---------------------------
RadiusClientStack::RadiusClientStack(uint16_t p_port, const char * p_hostname) :
    m_lastConnection(0),
    m_nextPacketID(0),
    m_connectionsNum(0),
    m_isValid(false)
{
    if (p_hostname != NULL)
        m_clientAddress.set(p_hostname, p_port);
    else
    {
        char l_hostname[200];
        gethostname(l_hostname,200);
        m_clientAddress.set(l_hostname, p_port);
    }

    // create UDP socket that we will use for communicating with RADIUS server
    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket == -1)
        return;

    // set REUSE ADDRESS option
#ifdef _MSC_VER
    BOOL l_reuseAddr = TRUE;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&l_reuseAddr, sizeof(l_reuseAddr)); 
#else
    int l_reuseAddr = 1;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &l_reuseAddr, sizeof(l_reuseAddr));
#endif

    // bind socket to given address and port
    struct sockaddr_in l_address;         // structure of address and port
    l_address.sin_family = AF_INET;
    l_address.sin_port = htons(p_port);
    if (p_hostname != NULL)
#if IN_ADDR_DEEPSTRUCT
        l_address.sin_addr.S_un.S_addr = m_clientAddress.getIP();
#else
        l_address.sin_addr.s_addr = m_clientAddress.getIP();
#endif
    else
#if IN_ADDR_DEEPSTRUCT
        l_address.sin_addr.S_un.S_addr = INADDR_ANY;
#else
        l_address.sin_addr.s_addr = INADDR_ANY;
#endif

    int l_rc = bind(m_socket, (struct sockaddr*)&l_address, sizeof(sockaddr_in));
    if (l_rc == -1)
        return;

    // make socket nonblocking
#ifdef _MSC_VER
    u_long l_blockMode = 1;
    ioctlsocket(m_socket, FIONBIO, &l_blockMode);
#else 
    int l_blockMode = 1;
    ioctl(m_socket, FIONBIO, &l_blockMode);
#endif

    m_isValid = true;
}


// ---------------------------
// Destructor
// ---------------------------
RadiusClientStack::~RadiusClientStack()
{
    if (m_socket != -1)
#ifdef _MSC_VER
        closesocket(m_socket);
#else
        close(m_socket);
#endif

    for (int i=0; i<m_connectionsNum; ++i)
        delete m_connections[i];
}


// ---------------------------
// Add RADIUS server to work with
// ---------------------------
int RadiusClientStack::addServer(const char * p_hostname, 
                                 uint16_t p_port, 
                                 RadiusSecret p_secret, 
                                 int p_responseTimeout,
                                 int p_retransmitCount)
{
    if (!isValid())
        return RC_FAIL;

    if (m_connectionsNum >= D_MAX_RADIUS_CLIENT_CONNECTIONS)
        return RC_FAIL;

    m_connections[m_connectionsNum] = new RadiusClientConnection(this, p_secret, p_hostname, p_port, p_responseTimeout, p_retransmitCount);
    ++m_connectionsNum;
    return RC_SUCCESS;
}

 
// ---------------------------
// Send packet to the network
// ---------------------------
RadiusPacket * RadiusClientStack::sendPacket(RadiusPacket & p_packet)
{
    RadiusPacket * l_response = NULL;
    int l_connectionsCount = 0;

    while (l_connectionsCount < m_connectionsNum)
    {
        l_response = m_connections[m_lastConnection]->sendPacket(p_packet);
        if (l_response != NULL)
            return l_response;

        // advance to the next server
        ++l_connectionsCount;
        ++m_lastConnection;
        if (m_lastConnection >= m_connectionsNum)
            m_lastConnection = 0;
    }

    return NULL;
}


// ---------------------------
// Get next ID for RADIUS packet
// ---------------------------
int RadiusClientStack::getNextPacketID()
{
    return m_nextPacketID++;
}


// ---------------------------
// Client stack was properly created?
// ---------------------------
bool RadiusClientStack::isValid()
{
    return m_isValid;
}


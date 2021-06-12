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

#include "RadiusServerStack.h" 


// ---------------------------
// Constructor
// ---------------------------
RadiusServerStack::RadiusServerStack(RadiusSecret p_secret, uint16_t p_port, const char * p_hostname) :
    m_request(D_PACKET_ACCESS_REQUEST),
    m_secret(p_secret),
    m_isValid(false)
{
    if (p_hostname != NULL)
        m_serverAddress.set(p_hostname, p_port);
    else
    {
        char l_hostname[200];
        gethostname(l_hostname,200);
        m_serverAddress.set(l_hostname, p_port);
    }

    // update secret key
    m_request.setSecret(&m_secret);

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
        l_address.sin_addr.S_un.S_addr = m_serverAddress.getIP();
#else
        l_address.sin_addr.s_addr = m_serverAddress.getIP();
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

    m_isValid = true;
}
 

// ---------------------------
// Destructor
// ---------------------------
RadiusServerStack::~RadiusServerStack()
{
    if (m_socket != -1)
#ifdef _MSC_VER
        closesocket(m_socket);
#else
        close(m_socket);
#endif
}


// ---------------------------
// Server stack was properly created?
// ---------------------------
bool RadiusServerStack::isValid()
{
    return m_isValid;
}


// ---------------------------
// Set blocking mode
// ---------------------------
int RadiusServerStack::setBlockMode(bool p_block)
{
    if (!isValid())
        return RC_FAIL;

    // make socket nonblocking
#ifdef _MSC_VER
    u_long l_blockMode = 0;
    if (p_block)
        l_blockMode = 1;

    int l_rc = ioctlsocket(m_socket, FIONBIO, &l_blockMode);
#else 
    int l_blockMode = 0;
    if (p_block)
        l_blockMode = 1;

    int l_rc = ioctl(m_socket, FIONBIO, &l_blockMode);
#endif
    if (l_rc == -1)
        return RC_FAIL;

    return RC_SUCCESS;
}


// ---------------------------
// Receive RADIUS request from network
// ---------------------------
int RadiusServerStack::receiveRequest()
{
    if (!isValid())
        return RC_FAIL;

    struct sockaddr_in l_address;
#if HAVE_SOCKLEN_T
    socklen_t l_addrLen;
#else
    int l_addrLen;
#endif
    l_addrLen = sizeof(struct sockaddr_in);

    int l_rc = recvfrom(
            m_socket,
            (char *)(m_request.getRawData()), 
            D_RADIUS_PACKET_MAX_LENGTH,
            0,
            (struct sockaddr*)&l_address,
            &l_addrLen);

    if (l_rc == -1)
        return RC_FAIL;

#if IN_ADDR_DEEPSTRUCT
    m_clientAddress.set(l_address.sin_addr.S_un.S_addr, ntohs(l_address.sin_port));
#else
    m_clientAddress.set(l_address.sin_addr.s_addr, ntohs(l_address.sin_port));
#endif
    return RC_SUCCESS;
}


// ---------------------------
// Get RADIUS request
// ---------------------------
RadiusPacket & RadiusServerStack::getRequest()
{
    return m_request;
}


// ---------------------------
// Get client address
// ---------------------------
RadiusAddress & RadiusServerStack::getClientAddress()
{
    return m_clientAddress;
}


// ---------------------------
// Send RADIUS response to the network
// ---------------------------
int RadiusServerStack::sendResponse(RadiusPacket & p_response, bool p_calculateAuthenticator)
{
    if (!isValid())
        return RC_FAIL;

    if (p_calculateAuthenticator)
        p_response.responseAuthenticator(); 

    struct sockaddr_in l_address;
#if HAVE_SOCKLEN_T
    socklen_t l_addrLen;
#else
    int l_addrLen;
#endif
    l_addrLen = sizeof(struct sockaddr_in);

    l_address.sin_family = AF_INET;
    l_address.sin_port = htons(m_clientAddress.getPort());
#if IN_ADDR_DEEPSTRUCT
    l_address.sin_addr.S_un.S_addr = m_clientAddress.getIP();
#else
    l_address.sin_addr.s_addr = m_clientAddress.getIP();
#endif

    int l_rc = sendto(
        m_socket, 
        (char *)p_response.getRawData(), 
        p_response.getLength(),
        0, 
        (struct sockaddr*)&l_address, 
        l_addrLen);

    if (l_rc == -1)
        return RC_FAIL;

    return RC_SUCCESS;
}


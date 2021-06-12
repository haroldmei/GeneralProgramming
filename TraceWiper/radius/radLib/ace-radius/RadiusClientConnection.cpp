/*
 * This source file is part of the ace-radius library.  This code was
 * written by Alex Agranov in 2004-2009, and is covered by the BSD open source
 * license. Refer to the accompanying documentation for details on usage and
 * license.
 */

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "RadiusClientConnection.h"
#include "RadiusClientStack.h"

#include <sys/types.h>
#ifdef _MSC_VER
    #include <winsock2.h>
#else
    #include <sys/socket.h>
#endif

#include <string.h>

// ---------------------------
// Constructor
// ---------------------------
RadiusClientConnection::RadiusClientConnection(
    RadiusClientStack * p_stack,
    const RadiusSecret & p_secret,
    const char * p_hostname, 
    uint16_t p_port,
    int p_responseTimeout, 
    int p_retransmitCount)
    :
    m_packet(D_PACKET_ACCESS_ACCEPT),
    m_stack(p_stack),
    m_secret(p_secret),
    m_responseTimeout(p_responseTimeout),
    m_retransmitCount(p_retransmitCount)
{
    m_serverAddress.set(p_hostname, p_port);
    m_packet.setSecret(&m_secret);
}


// ---------------------------
// Destructor
// ---------------------------
RadiusClientConnection::~RadiusClientConnection()
{
}


// ---------------------------
// Send packet
// ---------------------------
RadiusPacket * RadiusClientConnection::sendPacket(RadiusPacket & p_packet)
{
    int l_retransmitCount;
    int l_packetLength;
    struct sockaddr_in l_address;
#if HAVE_SOCKLEN_T
    socklen_t l_addrLen;
#else
    int l_addrLen;
#endif

    if (m_stack == NULL)
        return NULL;

    l_retransmitCount = 0;
    while (l_retransmitCount < m_retransmitCount)
    {
        // if packet has User-Password attribute, it should re-encode it
        if ((p_packet.getCode() == D_PACKET_ACCESS_REQUEST) ||
            (p_packet.getCode() == D_PACKET_ACCOUNTING_REQUEST))
            p_packet.setSecret(&m_secret);
    
        if (p_packet.getCode() == D_PACKET_ACCOUNTING_REQUEST)
            p_packet.accountingRequestAuthenticator();

        // re-build User-Password and Old-Password attributes
        p_packet.encodeUserPassword();

        // send request packet
        l_address.sin_family = AF_INET;
        l_address.sin_port = htons(m_serverAddress.getPort());
#if IN_ADDR_DEEPSTRUCT
        l_address.sin_addr.S_un.S_addr = m_serverAddress.getIP();
#else
        l_address.sin_addr.s_addr = m_serverAddress.getIP();
#endif
        l_addrLen = sizeof(struct sockaddr_in);

        int l_numBytesSent = sendto(
            m_stack->m_socket, 
            (char *)p_packet.getRawData(), 
            p_packet.getLength(),
            0, 
            (struct sockaddr*)&l_address, 
            l_addrLen);

        // wait for response
        if (l_numBytesSent != -1)
        {
            // call to API select (see man select)
#ifdef _MSC_VER
            int l_nfds = 0;
#else
            int l_nfds = m_stack->m_socket + 1;
#endif
            fd_set l_readSet;
            FD_ZERO(&l_readSet);
            FD_SET(m_stack->m_socket,&l_readSet);       // check ability of reading

            // set timeout
            struct timeval l_time;
            l_time.tv_sec = m_responseTimeout;
            l_time.tv_usec = 0;
            int l_canRead = select(l_nfds, &l_readSet, NULL, NULL, &l_time);
            FD_CLR(m_stack->m_socket, &l_readSet);

            if (l_canRead > 0)
            {
                // response arrived - receive it
                l_packetLength = recvfrom(
                    m_stack->m_socket,
                    (char *)(m_packet.getRawData()), 
                    D_RADIUS_PACKET_MAX_LENGTH,
                    0,
                    (struct sockaddr*)&l_address,
                    &l_addrLen);

                // if it's our response - return it
                if ((l_packetLength > 0) &&
                    (p_packet.verifyResponse(m_packet)))
                    return &m_packet;
            }
        }

        ++l_retransmitCount;
    }

    return NULL;
}


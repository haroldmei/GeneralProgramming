/*
 * This source file is part of the ace-radius library.  This code was
 * written by Alex Agranov in 2004-2009, and is covered by the BSD open source
 * license. Refer to the accompanying documentation for details on usage and
 * license.
 */

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "RadiusAddress.h"

#ifdef _MSC_VER
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <netdb.h>
#endif
#include <string.h>
#include <stdio.h>

#include <stdlib.h>

// ---------------------------
// Constructor
// ---------------------------
RadiusAddress::RadiusAddress() :
    m_IP(0),
    m_port(0) 
    {}

// ---------------------------
// Constructor
// ---------------------------
RadiusAddress::RadiusAddress(const char *p_hostname, uint16_t p_port)
{
    set(p_hostname, p_port);
}


// ---------------------------
// Get IP port
// ---------------------------
in_port_t RadiusAddress::getPort() const 
{ 
    return m_port; 
}

// ---------------------------
// Get IP address
// ---------------------------
in_addr_t RadiusAddress::getIP() const 
{ 
    return m_IP; 
}


// ---------------------------
// Update IP address & port
// ---------------------------
void RadiusAddress::set(in_addr_t p_hostname, uint16_t p_port)
{ 
    m_IP = p_hostname;
    m_port = p_port; 
}

// ---------------------------
// Update IP address & port
// ---------------------------
void RadiusAddress::set(const char *p_hostname, uint16_t p_port)
{
    m_IP = (in_addr_t)0;
    m_port = p_port; 

    if (p_hostname != NULL)
    {
#ifdef _MSC_VER
        unsigned long l_addr;
        l_addr = inet_addr(p_hostname);
        if (l_addr != INADDR_NONE)
        {
            m_IP = (in_addr_t)l_addr;
        }
        else
        {
            struct addrinfo * l_result = NULL;
            struct addrinfo * l_ptr = NULL;
            struct addrinfo l_hints;

            memset(&l_hints, 0, sizeof(l_hints) );
            l_hints.ai_family = AF_UNSPEC;
            l_hints.ai_socktype = SOCK_STREAM;
            l_hints.ai_protocol = IPPROTO_TCP;

            char l_port[20];
            //_itoa_s(p_port, l_port, sizeof(l_port), 10); 
            _itoa(p_port, l_port, sizeof(l_port)); 

            int l_ret = getaddrinfo(p_hostname, l_port, &l_hints, &l_result);
            if ( l_ret == 0 ) 
            {
                for(struct addrinfo *l_ptr=l_result; l_ptr != NULL; l_ptr=l_ptr->ai_next) 
                {
                    if (l_ptr->ai_family == AF_INET)
                    {
                        sockaddr_in * l_addr = (sockaddr_in *)l_ptr->ai_addr;
                        memcpy(&m_IP, &l_addr->sin_addr, sizeof(struct in_addr));
                        break;
                    }
                }
            }
        }
#else
        struct in_addr l_addr;
        if (inet_aton(p_hostname, &l_addr) != 0)
        {
            m_IP = l_addr.s_addr;
        }
        else
        {
            struct hostent * l_hostent;
            l_hostent = gethostbyname(p_hostname);
            if (l_hostent != NULL)
                memcpy(&m_IP, l_hostent->h_addr, l_hostent->h_length);
        }
#endif
    }
}


// Dump RadiusAddress in readable text format to STDOUT
void RadiusAddress::dump()
{
    struct in_addr l_addr;
    l_addr.s_addr = m_IP;
    printf("%s:%d", inet_ntoa(l_addr), m_port);
}

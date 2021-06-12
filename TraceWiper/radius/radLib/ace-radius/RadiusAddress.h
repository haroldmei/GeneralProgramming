/*
 * This source file is part of the ace-radius library.  This code was
 * written by Alex Agranov in 2004-2009, and is covered by the BSD open source
 * license. Refer to the accompanying documentation for details on usage and
 * license.
 */

#ifndef _RADIUSADDRESS_H__
#define _RADIUSADDRESS_H__

#ifdef _MSC_VER
    #include "Radius.h"
#else
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif

/**
 * @class RadiusAddress
 *
 * @brief Address of RADIUS server/client (IP address + port)
 */
class RadiusAddress
{
public:
    /// Constructor
    RadiusAddress();

    /// Constructor
    /**
     * @param[in] p_hostname - IP address or hostname
     * @param[in] p_port - IP port
     */
    RadiusAddress(const char *p_hostname, uint16_t p_port);

    /// Get IP port
    in_port_t getPort() const;
    /// Get IP address
    in_addr_t getIP() const;

    /// Update IP address & port
    void set(in_addr_t p_hostname, uint16_t p_port);
    /// Update IP address & port
    void set(const char *p_hostname, uint16_t p_port);

    /// Dump RadiusAddress in readable text format to STDOUT
    void dump();

private:
    in_addr_t  m_IP;    
    in_port_t  m_port;
};

#endif // _RADIUSADDRESS_H__


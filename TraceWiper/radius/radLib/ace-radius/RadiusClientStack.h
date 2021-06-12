/*
 * This source file is part of the ace-radius library.  This code was
 * written by Alex Agranov in 2004-2009, and is covered by the BSD open source
 * license. Refer to the accompanying documentation for details on usage and
 * license.
 */

#ifndef _RADIUSCLIENTSTACK_H__
#define _RADIUSCLIENTSTACK_H__

#include "Radius.h"
#include "RadiusPacket.h"
#include "RadiusAddress.h"
#include "RadiusAttribute.h"
#include "RadiusSecret.h"

#ifdef _MSC_VER
    #include <winsock2.h>
#endif

class RadiusClientConnection;

#define D_MAX_RADIUS_CLIENT_CONNECTIONS 10

/**
 * @class RadiusClientStack
 *
 * @brief Simple RADIUS client stack implementation
 *
 * A relatively simple fully-synchronic RADIUS client stack 
 * implementation. Packet transmission is performed on the 
 * context of the calling application thread and blocks until 
 * the response is received or (re)transmission times out. 
 *  
 * Multiple RADIUS server connections (with per-connection
 * secret keys and retransmission settings) are supported. Note,
 * however, that these multiple connections are used for 
 * high-availability and not for load-balancing. 
 *  
 * Packets are initially sent via the first connection. If 
 * corresponding RADIUS server doesn't respond, client stack 
 * switches to the next available connection. "Last known good" 
 * connection is used for transmission of consequent packets. 
 * Packet transmission is considered "failed" when all available
 * connections fail to transmit it. 
 *  
 * See test/client.cpp for a sample RADIUS client application.
 */
class RadiusClientStack
{
public:
    /// Constructor
    /** 
     * Both p_port and p_hostname parameters are optional and 
     * normally are not used.
     *  
     * @param[in] p_port      - local port of RADIUS client
     * @param[in] p_hostname  - local hostname of RADIUS client
     */
    RadiusClientStack(uint16_t p_port = 0, 
                      const char * p_hostname = NULL);


    /// Destructor
    ~RadiusClientStack();


    /// Client stack was properly created?
    bool isValid();


    /// Add RADIUS server to work with
    /**
     * At least one RADIUS server should be added to the RadiusClientStack
     * before transmitting any packet.
     *           
     * Multiple RADIUS servers (with per-server retransmission settings) may
     * be specified for high-availability (but not for load-balancing).           
     *                    
     * @return RC_SUCCESS if server was successfully added; RC_FAIL otherwise
     */
    int addServer(const char * p_hostname, 
                  uint16_t p_port, 
                  RadiusSecret p_secret, 
                  int p_responseTimeout = 5,
                  int p_retransmitCount = 2);


    /// Send packet to the network
    /**
     * Send packet to the RADIUS server and wait for response. If 
     * response is received - it's returned; otherwise NULL is returned. 
     *  
     * If more than one connection exists, packet will first be 
     * transmitted via the "last known good" connection. If the 
     * transmission fails, next connection will be tried. And so on 
     * and so forth. 
     *  
     * Only when all connections fail to transmit the packet, we 
     * give up and return NULL. 
     *
     * @param[in] p_packet - request packet to be sent
     * @return response packet received or NULL
     */
    RadiusPacket * sendPacket(RadiusPacket & p_packet);


    /// Get next ID for RADIUS packet
    /**
     * @return next packet ID
     */
    int getNextPacketID();


private:

    friend class RadiusClientConnection;

    /// Client connections
    RadiusClientConnection * m_connections[D_MAX_RADIUS_CLIENT_CONNECTIONS];

    /// Number of client connections
    int m_connectionsNum;
    
    /// Last used client connection
    int m_lastConnection;
    
    /// Next packet ID
    int m_nextPacketID;

    /// Client address
    RadiusAddress   m_clientAddress;

    /// UDP socket
#ifdef _MSC_VER
    SOCKET  m_socket; 
#else
    int  m_socket; 
#endif

    /// Socket was properly created?
    bool m_isValid;
};



#endif // _RADIUSCLIENTSTACK_H__


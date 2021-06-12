/*
 * This source file is part of the ace-radius library.  This code was
 * written by Alex Agranov in 2004-2009, and is covered by the BSD open source
 * license. Refer to the accompanying documentation for details on usage and
 * license.
 */

#ifndef _RADIUSSERVERSTACK_H__
#define _RADIUSSERVERSTACK_H__

#include "Radius.h"
#include "RadiusPacket.h"
#include "RadiusAddress.h"
#include "RadiusAttribute.h"
#include "RadiusSecret.h"

#ifdef _MSC_VER
    #include <winsock2.h>
#endif

/**
 * @class RadiusServerStack
 *
 * @brief Simple RADIUS server stack implementation
 *
 * A relatively simple fully-synchronic RADIUS server stack 
 * implementation. Packet transmission is performed on the 
 * context of the calling application thread. 
 *  
 * Typical server application should do the following sequence 
 * of actions in a loop: 
 *    - call receiveRequest() to receive RADIUS request
 *    - parse request and build response packet
 *    - call sendResponse() to send RADIUS response
 *  
 * Application may choose whether receivePacket() blocks 
 * until new packet is available or not - refer to 
 * setBlockMode() method. 
 *  
 * See test/server.cpp for a sample RADIUS server application.
 */
class RadiusServerStack
{
public:
    /// Constructor
    /** 
     * Both p_port and p_hostname parameters are optional and 
     * normally are not used.
     *  
     * @param[in] p_secret    - secret key 
     * @param[in] p_port      - local port of RADIUS server
     * @param[in] p_hostname  - local hostname of RADIUS server (if 
     *       not specified, RADIUS server will listen to all local
     *       IP addresses)
     */
    RadiusServerStack(RadiusSecret p_secret,
                      uint16_t p_port = D_RADIUS_AUTHENTICATION_PORT, 
                      const char * p_hostname = NULL);


    /// Destructor
    ~RadiusServerStack();


    /// Server stack was properly created?
    bool isValid();


    /// Set blocking mode
    /**
     * Blocking mode determines whether receivePacket() call blocks 
     * until new packet is available or immediately returns with a 
     * failure code. By default blocking mode is turned on, but 
     * application may use this API to change it. 
     *                    
     * @param[in] p_block    - blocking mode (true/false)
     * @return RC_SUCCESS if blocking mode was successfully set; 
     *         RC_FAIL otherwise
     */
    int setBlockMode(bool p_block);


    /// Receive RADIUS request from network
    /**
     * Receive RADIUS request from the network. 
     *  
     * If new packet is available, RC_SUCCESS is returned and 
     * received packet is accessible via getRequest() method.
     * 
     * Otherwise, depending on the blocking mode, this method
     * either blocks until the new packet is available, or 
     * immediately returns RC_FAIL. 
     *                    
     * @return RC_SUCCESS if new request was received; RC_FAIL 
     *         otherwise
     */
    int receiveRequest();


    /// Get RADIUS request
    /**
     * Get last RADIUS request received from the network. 
     * 
     * @return request received from the network
     */
    RadiusPacket & getRequest();


    /// Get client address
    /**
     * Get address of the RADIUS client that the last request was 
     * received from. 
     * 
     * @return address of RADIUS client
     */
    RadiusAddress & getClientAddress();

    /// Send RADIUS response to the network
    /**
     * Send RADIUS response to the network. 
     *
     * @param[in] p_response - response packet to be sent
     * @param[in] p_calculateAuthenticator - calculate response 
     *       authenticator prior to sending it
     * @return RC_SUCCESS if response packet was successfully sent; 
     *         RC_FAIL otherwise
     */
    int sendResponse(RadiusPacket & p_response, bool p_calculateAuthenticator = true);

private:

    /// Server address
    RadiusAddress   m_serverAddress;

    /// UDP socket
#ifdef _MSC_VER
    SOCKET m_socket;
#else
    int  m_socket; 
#endif

    /// Socket was properly created?
    bool m_isValid;

    /// Request received from the network
    RadiusPacket  m_request; 

    /// Secret key
    RadiusSecret  m_secret; 

    /// Client address
    RadiusAddress   m_clientAddress;
};



#endif // _RADIUSSERVERSTACK_H__


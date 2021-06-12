/*
 * This source file is part of the ace-radius library.  This code was
 * written by Alex Agranov in 2004-2009, and is covered by the BSD open source
 * license. Refer to the accompanying documentation for details on usage and
 * license.
 */

#ifndef _RADIUSPACKET_H__
#define _RADIUSPACKET_H__

#include "RadiusAttribute.h"


// Radius packet code values (from RFC2865)
#define D_PACKET_ACCESS_REQUEST         1
#define D_PACKET_ACCESS_ACCEPT          2
#define D_PACKET_ACCESS_REJECT          3
#define D_PACKET_ACCOUNTING_REQUEST     4
#define D_PACKET_ACCOUNTING_RESPONSE    5
#define D_PACKET_ACCESS_CHALLENGE       11
#define D_PACKET_STATUS_SERVER          12
#define D_PACKET_STATUS_CLIENT          13

#define D_PACKET_MAX_KNOWN_CODE         13



// Some global constants (following RFC definitions)
#define D_RADIUS_PACKET_HEADER_LENGTH 20
#define D_RADIUS_PACKET_MAX_LENGTH    4096
#define D_AUTHENTICATOR_LENGTH        16

// Raw data buffer is used for storing the whole RADIUS packet _AND_ secret key
// (hence it should be D_RADIUS_PACKET_MAX_LENGTH + D_SECRET_MAX_LENGTH)
#define D_RAW_DATA_LENGTH           4352


class RadiusSecret;
class RadiusClientStack;

/**
 * @class RadiusPacket
 *
 * @brief Implementation of RADIUS packet
 *
 * RadiusPacket class is responsible for holding data transferred
 * to/from the network and providing simple interface to build new
 * outgoing RADIUS packet and to parse incoming packet.
 *
 */
class RadiusPacket
{
public:

    /// Constructor
    /**
     * This constructor should be used for constructing RADIUS 
     * requests (i.e. by RADIUS client application). It 
     * automatically generates random Authenticator, however 
     * application may override this later on by calling 
     * setAuthenticator() method. 
     *
     * @param[in] p_code - packet code
     */
    RadiusPacket(unsigned char p_code, RadiusClientStack & p_stack);

    /// Constructor
    /**
     * This constructor should be used for constructing RADIUS 
     * responses (i.e. by RADIUS server application). Original 
     * RADIUS request should be passed as parameter to it. 
     *
     * @param[in] p_code - packet code
     * @param[in] p_request - original RADIUS request packet
     */
    RadiusPacket(unsigned char p_code, RadiusPacket & p_request);
    
    /// Constructor
    /**
     * This constructor should be used for constructing RADIUS requests.
     * It automatically generates random Authenticator, however application
     * may override this later on by calling setAuthenticator() method. 
     *  
     * Most applications should use the former constructors instead 
     * of this one. If for any reason you decide to use this 
     * constructor, make sure to update packet's identifier (by 
     * default packet will have zero ID). 
     *
     * @param[in] p_code - packet code
     */
    RadiusPacket(unsigned char p_code);


    /// Copy constructor
    RadiusPacket(const RadiusPacket & p_packet);

    /// Destructor
    ~RadiusPacket();


    /// Get packet code
    /**
     * @return packet code
     */
    unsigned char getCode(void);

    /// Get packet code description
    const char * getCodeDescription(void);

    
    /// Get packet length
    /**
     * @return packet length
     */
    uint16_t getLength(void);



    /// Get packet identifier
    /**
     * @return packet identifier
     */
    unsigned char getID(void);
    
    /// Set packet identifier
    /**
     * @param[in] p_id - packet identifier
     */
    void setID(unsigned char p_id);

    /// Set random packet identifier
    void randomID();


    /// Get secret key used for packet encoding
    /**
     * @return secret key
     */
    RadiusSecret * getSecret(void);
    
    /// Set secret key used for packet encoding
    /**
     * @param[in] p_secret - secret key
     */
    void setSecret(RadiusSecret * p_secret);


    /// Get packet authenticator
    const unsigned char * getAuthenticator(void);
    
    /// Set packet authenticator
    void setAuthenticator(const unsigned char * p_auth);

    

    /// Dump radius packet in readable text format to the STDOUT
    void dump();
    
    
    /// Get first attribute in the packet
    /**
     * Updates p_attr to point at the first attribute in the packet.
     * If packet has no attributes "invalid" attribute is returned. 
     *
     * Typical code that goes over all attributes in the packet looks as follows:
     * \code
     *     RadiusAttribute l_attr;
     *     for (packet->getFirstAttribute(l_attr); 
     *          l_attr.isValid(); 
     *          packet->getNextAttribute(l_attr))
     *     {
     *         // do something with the attribute
     *     }
     * \endcode
     *      
     * Application can analyze return code or use isValid() method of RadiusAttribute
     * to verify validity of the returned attribute.
     *
     * @param[out] p_attr - first attribute in the packet
     * @return RC_SUCCESS or RC_FAIL
     */
    int getFirstAttribute(RadiusAttribute & p_attr);


    /// Get next attribute in the packet
    /**
     * Updates p_attr to point at the next attribute in the packet after the
     * one that it was pointing at prior to calling this function.
     * If no more attributes are found "invalid" attribute is returned.
     *
     * Application can analyze return code or use isValid() method of RadiusAttribute
     * to verify validity of the returned attribute.
     *
     * @param[in,out] p_attr - input - current attribute; output - next attribute
     * @return RC_SUCCESS or RC_FAIL
     */
    int getNextAttribute(RadiusAttribute & p_attr);


    /// Find specific attribute in the packet
    /**
     * Updates p_attr to point at the first attribute of specified type in the packet.
     * If attribute is not found "invalid" attribute is returned.
     * If more than one instance of the same attribute exist in the packet,
     * application should use getNextAttribute() interface to retrieve them.
     *
     * Application can analyze return code or use isValid() method of RadiusAttribute
     * to verify validity of the returned attribute.
     *
     * @param[in,out] p_attr - input - current attribute; output - next attribute
     * @return RC_SUCCESS or RC_FAIL
     */
    int findAttribute(unsigned char p_attrType, RadiusAttribute & p_attr);


    /// Add new attribute to the packet
    /**
     * Updates p_attr to point at the free space in the packet.
     * Returns RadiusAttribute structure should be used by application in order to
     * set attribute's value. If packet is full, "invalid" attribute is returned.
     *
     * Actual attribute's data is not encoded into the packet until proper attribute's 
     * set() method is called. So if application calls addAttribute() and then suddenly 
     * changes it's mind, it's perfectly OK to simply call addAttribute() again 
     * or send the packet "as is".
     *
     * Application can analyze return code or use isValid() method of RadiusAttribute
     * to verify validity of the returned attribute.
     *
     * @param[in]  p_attrType - attribute type
     * @param[out] p_attr - reference to the added attribute
     * @return RC_SUCCESS or RC_FAIL
     */
    int addAttribute(unsigned char p_attrType, RadiusAttribute & p_attr);

    
    /// Get raw packet data
    unsigned char * getRawData(void);


    /// Adjust packet's length
    /**
     * Adjust (increase) packet's length by specified number of bytes.
     * If packet has not enough space to accomodate requested data, RC_FAIL is returned.
     *
     * @param[in] p_length - how many bytes should be added to the packet
     * @return RC_SUCCESS or RC_FAIL
     */
    int adjustLength(uint16_t p_length);


    /// Generate authenticator for response packet
    /**
     * Packet's authenticator should be initially set to the request's authenticator.
     * This method should be called once only - prior to sending the packet over
     * the network. Usually RadiusServer does the job, so user application doesn't
     * have to bother.
     *
     * @param[in] p_secret - secret key (if NULL, stack's secret key is taken)
     */
    void responseAuthenticator(RadiusSecret * p_secret = NULL);


    // Encode User-Password attribute
    /*
     * When packet is retransmitted through new connection, there may be a need to reencode
     * User-Password attribute, since secret key of the new connection may be different.
     * That's exacly what this method does - reencodes User-Password attribute, if the
     * latter exists in this packet.
     *
     * RadiusClientConnection class uses this method. 
     * There seems to be no reason why user application would use this API.
     */
    void encodeUserPassword(void);


    /// Generate authenticator for accounting request packet
    /**
     * Packet's attributes should be ready before calling
     * This method should be called once only - prior to sending the packet over
     * the network. Usually RadiusClientStack does the job, so user application doesn't
     * have to bother.
     *
     * @param[in] p_secret - secret key (if NULL, stack's secret key is taken)
     */
    void accountingRequestAuthenticator(RadiusSecret * p_secret = NULL);


    /// Verify RADIUS response
    bool verifyResponse(RadiusPacket & p_packet);

private:

    friend class RadiusAttribute;

    /// Init packet
    void init(unsigned char p_code);

    /// Generate pseudo-random authenticator
    void randomAuthenticator(void);

    
    /// Set packet's code
    /**
     * Set packet's code.
     * This method is intentionally put into the private section, since application should
     * pass packet's code to RadiusPacket constructor.
     * 
     * @param[in] p_code - packet code
     */
    void setCode(unsigned char p_code);

    
    /// Set packet's length
    /**
     * Set packet's length
     * 
     * @param[in] p_length - packet's length
     */
    void setLength(uint16_t p_length);



    /// Raw packet data
    unsigned char m_data[D_RAW_DATA_LENGTH];


    /// RADIUS secret associated with this packet
    RadiusSecret * m_secret;


    // User-Password attribute in unencrypted format
    /*
     * We need this data in order to re-encode User-Password attribute prior to retransmitting it
     * through another connection (when more than one RadiusClientConnection is defined);
     */
    unsigned char m_password[D_USER_PASSWORD_MAX_LENGTH];
    uint16_t    m_passwordLength;
    uint16_t    m_passwordOffset;

    // Old-Password attribute in unencrypted format
    /*
     * We need this data in order to re-encode Old-Password attribute prior to retransmitting it
     * through another connection (when more than one RadiusClientConnection is defined);
     */
    unsigned char m_oldPassword[D_USER_PASSWORD_MAX_LENGTH];
    uint16_t    m_oldPasswordLength;
    uint16_t    m_oldPasswordOffset;
};

#endif // _RADIUSPACKET_H__


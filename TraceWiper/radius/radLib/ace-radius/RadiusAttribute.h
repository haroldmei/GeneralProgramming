/*
 * This source file is part of the ace-radius library.  This code was
 * written by Alex Agranov in 2004-2009, and is covered by the BSD open source
 * license. Refer to the accompanying documentation for details on usage and
 * license.
 */

#ifndef _RADIUSATTRIBUTE_H__
#define _RADIUSATTRIBUTE_H__

#include "Radius.h"

// definition of Attribute Types from RFC2865
#define D_ATTR_USER_NAME                1
#define D_ATTR_USER_PASSWORD            2
#define D_ATTR_CHAP_PASSWORD            3
#define D_ATTR_NAS_IP_ADDRESS           4
#define D_ATTR_NAS_PORT                 5
#define D_ATTR_SERVICE_TYPE             6
#define D_ATTR_FRAMED_PROTOCOL          7
#define D_ATTR_FRAMED_IP_ADDRESS        8
#define D_ATTR_FRAMED_IP_NETMASK        9
#define D_ATTR_FRAMED_ROUTING           10 
#define D_ATTR_FILTER_ID                11 
#define D_ATTR_FRAMED_MTU               12 
#define D_ATTR_FRAMED_COMPRESSION       13 
#define D_ATTR_LOGIN_IP_HOST            14 
#define D_ATTR_LOGIN_SERVICE            15 
#define D_ATTR_LOGIN_TCP_PORT           16 
#define D_ATTR_OLD_PASSWORD             17
#define D_ATTR_REPLY_MESSAGE            18 
#define D_ATTR_CALLBACK_NUMBER          19 
#define D_ATTR_CALLBACK_ID              20 
#define D_ATTR_FRAMED_ROUTE             22 
#define D_ATTR_FRAMED_IPX_NETWORK       23 
#define D_ATTR_STATE                    24 
#define D_ATTR_CLASS                    25 
#define D_ATTR_VENDOR_SPECIFIC          26 
#define D_ATTR_SESSION_TIMEOUT          27 
#define D_ATTR_IDLE_TIMEOUT             28 
#define D_ATTR_TERMINATION_ACTION       29 
#define D_ATTR_CALLED_STATION_ID        30 
#define D_ATTR_CALLING_STATION_ID       31 
#define D_ATTR_NAS_IDENTIFIER           32 
#define D_ATTR_PROXY_STATE              33 
#define D_ATTR_LOGIN_LAT_SERVICE        34 
#define D_ATTR_LOGIN_LAT_NODE           35 
#define D_ATTR_LOGIN_LAT_GROUP          36 
#define D_ATTR_FRAMED_APPLETALK_LINK    37 
#define D_ATTR_FRAMED_APPLETALK_NETWORK 38 
#define D_ATTR_FRAMED_APPLETALK_ZONE    39 
// types 40-59 are reserved for accounting - RFC2866
#define D_ATTR_CHAP_CHALLENGE           60 
#define D_ATTR_NAS_PORT_TYPE            61 
#define D_ATTR_PORT_LIMIT               62 
#define D_ATTR_LOGIN_LAT_PROMPT         63

// definition of Attribute Types from RFC2866
#define D_ATTR_ACCT_STATUS_TYPE         40 
#define D_ATTR_ACCT_DELAY_TIME          41 
#define D_ATTR_ACCT_INPUT_OCTETS        42 
#define D_ATTR_ACCT_OUTPUT_OCTETS       43 
#define D_ATTR_ACCT_SESSION_ID          44 
#define D_ATTR_ACCT_AUTHENTIC           45 
#define D_ATTR_ACCT_SESSION_TIME        46 
#define D_ATTR_ACCT_INPUT_PACKETS       47 
#define D_ATTR_ACCT_OUTPUT_PACKETS      48 
#define D_ATTR_ACCT_TERMINATE_CAUSE     49 
#define D_ATTR_ACCT_MULTI_SESSION_ID    50 
#define D_ATTR_ACCT_LINK_COUNT          51 

#define D_ATTR_MAX_KNOWN_TYPE           63



// definition of Acct-Status-Type attribute values from RFC2866
#define D_ACCT_STATUS_START             1
#define D_ACCT_STATUS_STOP              2
#define D_ACCT_STATUS_INTERIM_UPDATE    3
#define D_ACCT_STATUS_ON                7
#define D_ACCT_STATUS_OFF               8


// max length of User-Password attribute
#define D_USER_PASSWORD_MAX_LENGTH    128



class RadiusPacket;
class RadiusSecret;

/**
 * @class RadiusAttribute
 *
 * @brief Implementation of RADIUS attribute
 *
 * This class does not keep any data - instead it provides methods to
 * properly access and encode data at specific offset inside the RADIUS
 * packet raw data buffer. 
 *
 * This implementation minimizes the footprint of RADIUS stack and
 * does not require dynamic memory allocations. It's also very fast
 * (at least in the typical RADIUS applications) since the data is
 * immediately encoded inside the packet.
 *
 */
class RadiusAttribute
{
public:

    /// Constructor
    RadiusAttribute();
    
    /// Destructor
    ~RadiusAttribute();

    
    /// Dump attribute in readable text format to the STDOUT
    void dump();

    
    /// Init attribute
    /**
     * Set associated RADIUS packet and offset in it - this will effectively
     * init the attribute. A special usage is to pass zero p_offset - this
     * will mark attribute "invalid".
     *
     * @param[in] p_packet - RADIUS packet
     * @param[in] p_offset - offset inside the packet's raw data
     */
    void init(RadiusPacket * p_packet, uint16_t p_offset);

    
    /// Get packet that this attribute belongs to
    RadiusPacket * getPacket(void);


    /// Get attribute's offset inside the packet
    uint16_t getOffset(void);


    ///////////////////////////////////////////////////////////////////////

    /// Get attribute type
    /**
     * @return attribute type
     */
    unsigned char getType(void);

    /// Set attribute type
    /**
     * Set attribute type
     *
     * @param[in] p_code - attribute code
     * @return RC_SUCCESS or RC_FAIL
     */
    int setType(unsigned char p_type);

    
    /// Get textual description of attribute type
    const char * getTypeDescription(void);


    /// Get attribute length
    /**
     * Note that returned value is length of the attribute data block 
     * as encoded in raw RADIUS packet (in other words it's simply
     * a value of Length octet in attribute data).
     * You must take into account specific attribute type in order to
     * calculate the "real data" length.
     *
     * @return attribute's length
     */
    unsigned char getLength(void);

    /// Set attribute length
    /**
     * Set attribute's length - refer to getLength() description for details.
     *
     * @param[in] p_length - attribute's length
     * @return RC_SUCCESS or RC_FAIL
     */
    int setLength(unsigned char p_length);

    
    /// Attribute is valid?
    /**
     * @return TRUE or FALSE
     */
    int isValid(void);


    // ------------------------------------------------------------
    // get- and set- methods - access data stored in the attribute

    // ===========================
    // Regular attributes
    // ===========================

    /// Get value of the numeric attribute
    /**
     * @return value of numeric attribute in host order format
     */
    uint32_t getNumber(void);
    
    /// Set value of the numeric attribute
    /**
     * @param[in] p_value - numeric value in host order format
     * @return RC_SUCCESS if attribute is successfully encode; RC_FAIL otherwise
     */
    int setNumber(uint32_t p_value);


    /// Get value of the IP address attribute
    /**
     * @return value of IP address attribute
     */
    struct in_addr getIPAddress(void);

    /// Set value of the IP address attribute
    /**
     * @param[in] p_value - IP address
     * @return RC_SUCCESS if attribute is successfully encode; RC_FAIL otherwise
     */
    int setIPAddress(struct in_addr p_value);


    /// Get value of the string attribute
    /**
     * Note that string data may contain both readable characters 
     * and non-readable binary data. In any case string will not be 
     * null-terminated, hence you should use returned p_length value 
     * to properly operate on it. 
     *
     * @param[out] p_data   - pointer to the beginning of string data
     * @param[out] p_length - string length
     * @return RC_SUCCESS or RC_FAIL
     */
    int getString(const char * & p_data, uint16_t & p_length);
    
    /// Set value of the string attribute
    /**
     * Note that string data may contain both readable characters 
     * and non-readable binary data and in any case encoded string 
     * should not be null-terminated. 
     *  
     * You have two options: 
     *    - provide a "raw" buffer and its length
     *    - provide NULL-terminated string and leave the p_length
     *      parameter unspecified (in this case strlen() will be
     *      used to determine encoded string length)
     *
     * @param[in] p_data   - pointer to the beginning of string data
     * @param[in] p_length - string length (optional)
     * @return RC_SUCCESS or RC_FAIL
     */
    int setString(const char * p_data, uint16_t p_length = 0);


    // ===========================
    // Vendor-Specific attribute
    // ===========================

    /// Get vendor ID of Vendor-Specific attribute
    /**
     * @return vendor ID numeric value in host order format
     */
    uint32_t getVendorId(void);
    
    /// Set vendor ID of Vendor-Specific attribute
    /**
     * @param[in] p_vendorId - vendor ID numeric value in host order format
     * @return RC_SUCCESS or RC_FAIL
     */
    int setVendorId(uint32_t p_vendorId);

    /// Get "raw" string data of Vendor-Specific attribute
    /**
     * "Raw" string data of Vendor-Specific attribute typically 
     * contains Vendor Type, Vendor Length and Attribute-specific 
     * data. Therefore in most cases you should use getVendorType(),
     * getVendorLength() and getVendorNumber() instead of this 
     * method. 
     *  
     * Note that string data may contain both readable characters and non-readable
     * binary data. In any case string will not be null-terminated, 
     * hence yous should use returned p_length value to properly
     * operate on it. 
     *
     * @param[out] p_data   - pointer to the beginning of string data
     * @param[out] p_length - string length
     * @return RC_SUCCESS or RC_FAIL
     */
    int getVendorRawString(const char * & p_data, uint16_t & p_length);
    
    /// Set "raw" string data of Vendor-Specific attribute
    /**
     * "Raw" string data of Vendor-Specific attribute typically 
     * contains Vendor Type, Vendor Length and Attribute-specific 
     * data. Therefore in most cases you should use setVendorType(),
     * setVendorLength() and setVendorNumber() instead of this 
     * method. 
     *  
     * Note that string data may contain both readable characters 
     * and non-readable binary data and in any case encoded string 
     * should not be null-terminated. 
     *  
     * You have two options: 
     *    - provide a "raw" buffer and its length
     *    - provide NULL-terminated string and leave the p_length
     *      parameter unspecified (in this case strlen() will be
     *      used to determine encoded string length)
     *
     * @param[in] p_data   - pointer to the beginning of string data
     * @param[in] p_length - string length (optional)
     * @return RC_SUCCESS or RC_FAIL
     */
    int setVendorRawString(const char * p_data, uint16_t p_length = 0);


    /// Get vendor type of Vendor-Specific attribute
    /**
     * @return vendor type
     */
	unsigned char getVendorType();

    /// Set vendor type of Vendor-Specific attribute
    /**
     * @param[in] p_value - vendor type value
     * @return RC_SUCCESS or RC_FAIL
     */
	int setVendorType(unsigned char p_value);


    /// Get vendor length of Vendor-Specific attribute
    /**
     * @return vendor length
     */
	unsigned char getVendorLength();

    /// Set vendor length of Vendor-Specific attribute
    /**
     * @param[in] p_length - vendor length value
     * @return RC_SUCCESS or RC_FAIL
     */
	int setVendorLength(unsigned char p_length);


    /// Get value of the numeric Vendor-Specific attribute
    /**
     * @return value of numeric attribute in host order format
     */
	uint32_t getVendorNumber();
    
    /// Set value of the numeric Vendor-Specific attribute
    /**
     * @param[in] p_value - numeric value in host order format
     * @return RC_SUCCESS if attribute is successfully encode; RC_FAIL otherwise
     */
	int setVendorNumber(uint32_t p_value);


    /// Get value of the IP address Vendor-Specific attribute
    /**
     * @return value of IP address attribute
     */
	struct in_addr getVendorIPAddress();   

    /// Set value of the IP address Vendor-Specific attribute
    /**
     * @param[in] p_value - IP address
     * @return RC_SUCCESS if attribute is successfully encode; RC_FAIL otherwise
     */
	int setVendorIPAddress(struct in_addr p_address);


    /// Get value of the string Vendor-Specific attribute
    /**
     * Note that string data may contain both readable characters 
     * and non-readable binary data. In any case string will not be 
     * null-terminated, hence you should use returned p_length value 
     * to properly operate on it. 
     *
     * @param[out] p_data   - pointer to the beginning of string data
     * @param[out] p_length - string length
     * @return RC_SUCCESS or RC_FAIL
     */
	int getVendorString(const char * & p_data, uint16_t & p_length);
    
    /// Set value of the string Vendor-Specific attribute
    /**
     * Note that string data may contain both readable characters and non-readable
     * binary data. In any case string should not be null-terminated.
     *
     * Note that string data may contain both readable characters 
     * and non-readable binary data and in any case encoded string 
     * should not be null-terminated. 
     *  
     * You have two options: 
     *    - provide a "raw" buffer and its length
     *    - provide NULL-terminated string and leave the p_length
     *      parameter unspecified (in this case strlen() will be
     *      used to determine encoded string length)
     *
     * @param[in] p_data   - pointer to the beginning of string data
     * @param[in] p_length - string length (optional)
     * @return RC_SUCCESS or RC_FAIL
     */
	int setVendorString(const char * p_data, uint16_t p_length = 0);


    // ===========================
    // User-Password attribute
    // ===========================
    
    /// Get string data of User-Password attribute
    /**
     * Password is decoded from the packet using secret key and presented in 
     * it's "original" form.
     *  
     * Note that password may contain both readable characters and 
     * non-readable binary data. In any case string will not be 
     * null-terminated, hence you should use returned p_length value
     * to properly operate on it. 
     *
     * @param[in] p_data   - pointer to local buffer allocated by application
     *                        where the decoded password will be stored
     * @param[out] p_length - string length
     * @param[in] p_secret - secret key (if NULL, stack's secret key is taken)
     * @return RC_SUCCESS or RC_FAIL
     *
     * IMPORTANT: allocate the buffer of size  D_USER_PASSWORD_MAX_LENGTH 
     *            for storing decoded password data and pass it as
     *            p_data
     */
    int getUserPassword(char * p_data, uint16_t & p_length, 
                        RadiusSecret * p_secret = NULL);
    
    /// Set string data of User-Password attribute
    /**
     * Password is encoded into the packet using secret key. 
     * The actual encoding is performed when packet is transmitted 
     * to the network (using the secret key of current connection) -
     * therefore if you "dump" packet beforehand you won't see 
     * any data encoded. 
     *  
     * Note that the password may contain both readable characters 
     * and non-readable binary data. In any case encoded string 
     * should not be null-terminated. 
     *  
     * You have two options: 
     *    - provide a "raw" buffer and its length
     *    - provide NULL-terminated string and leave the p_length
     *      parameter unspecified (in this case strlen() will be
     *      used to determine encoded password length)
     *  
     * @param[in] p_data   - pointer to the beginning of string data
     * @param[in] p_length - string length
     * @return RC_SUCCESS or RC_FAIL
     */
    int setUserPassword(const char * p_data, uint16_t p_length = 0);

    // Encode raw data of User-Password attribute
    /*
     * We need this function in order to re-encode User-Password attribute with new secret key
     * when packet is retransmitted through new connection; application should not use this API.
     */
    int encodeUserPassword(const unsigned char * p_data, uint16_t p_length);


    // ===========================
    // Old-Password attribute
    // ===========================

    /// Get string data of Old-Password attribute
    /**
     * Password is decoded from the packet using secret key and presented in 
     * it's "original" form.
     *  
     * Note that string data may contain both readable characters 
     * and non-readable binary data. In any case string will not be 
     * null-terminated, hence you should use returned p_length value 
     * to properly operate on it. 
     *
     * @param[in] p_data   - pointer to local buffer allocated by application
     *                        where the decoded password will be stored
     * @param[out] p_length - string length
     * @param[in] p_secret - secret key (if NULL, stack's secret key is taken)
     * @return RC_SUCCESS or RC_FAIL
     *
     * IMPORTANT: allocate the buffer of size D_USER_PASSWORD_MAX_LENGTH 
     *            for storing decoded password data and pass it as
     *            p_data
     */
    int getOldPassword(char * p_data, uint16_t & p_length, 
                        RadiusSecret * p_secret = NULL);

    /// Set string data of Old-Password attribute
    /**
     * Password is encoded into the packet using secret key.
     * The actual encoding is performed when packet is transmitted 
     * to the network (using the secret key of current connection) -
     * therefore if you "dump" packet beforehand you won't see 
     * any data encoded. 
     * 
     * The application must add User-Password attribute prior to 
     * adding Old-Password, since encrypted User-Password data is 
     * used instead of shared authenticator for encoding 
     *  
     * Note that the password may contain both readable characters 
     * and non-readable binary data. In any case encoded string 
     * should not be null-terminated. 
     *  
     * You have two options: 
     *    - provide a "raw" buffer and its length
     *    - provide NULL-terminated string and leave the p_length
     *      parameter unspecified (in this case strlen() will be
     *      used to determine encoded password length)
     *
     * @param[in] p_data   - pointer to the beginning of string data
     * @param[in] p_length - string length
     * @return RC_SUCCESS or RC_FAIL
     */
    int setOldPassword(const char * p_data, uint16_t p_length = 0);

    // Encode raw data of Old-Password attribute
    /*
     * We need this function in order to re-encode User-Password attribute with new secret key
     * when packet is retransmitted through new connection; application should not use this API.
     */
    int encodeOldPassword(const unsigned char * p_data, uint16_t p_length);


    // ===========================
    // CHAP-Password attribute
    // ===========================

    /// Get CHAP identifier of CHAP-Password attribute
    /**
     * @return CHAP identifier
     */
    unsigned char getChapIdentifier(void);
    
    /// Set vendor ID of CHAP-Password attribute
    /**
     * @param[in] p_chapIdentifier - CHAP identifier value
     * @return RC_SUCCESS or RC_FAIL
     */
    int setChapIdentifier(unsigned char p_chapId);

    /// Get string data of CHAP-Password attribute
    /**
     * Note that string data may contain both readable characters 
     * and non-readable binary data. In any case string will not be 
     * null-terminated, hence you should use returned p_length value 
     * to properly operate on it. 
     *
     * @param[out] p_data   - pointer to the beginning of string data
     * @param[out] p_length - string length
     * @return RC_SUCCESS or RC_FAIL
     */
    int getChapString(const char * & p_data, uint16_t & p_length);
    
    /// Set string data of CHAP-Password attribute
    /**
     * Note that string data may contain both readable characters and non-readable
     * binary data. In any case string should not be null-terminated.
     *
     * You have two options: 
     *    - provide a "raw" buffer and its length
     *    - provide NULL-terminated string and leave the p_length
     *      parameter unspecified (in this case strlen() will be
     *      used to determine encoded password length)
     *
     * @param[in] p_data   - pointer to the beginning of string data
     * @param[in] p_length - string length
     * @return RC_SUCCESS or RC_FAIL
     */
    int setChapString(const char * p_data, uint16_t p_length = 0);



private:

    // Get pointer to the attribute raw data
    unsigned char * getRawData(void);

    // Types of attribute data
    typedef enum AttributeFormat_e
    {
        E_ATTR_FORMAT_INTEGER,
        E_ATTR_FORMAT_IP_ADDRESS,
        E_ATTR_FORMAT_STRING,
        E_ATTR_FORMAT_VENDOR_SPECIFIC,
        E_ATTR_FORMAT_USER_PASSWORD,
        E_ATTR_FORMAT_CHAP_PASSWORD
    };

    // Dump attribute in readable text format to the STDOUT
    /**
     * @param[in] p_format - attribute format; 
     */
    void dump(AttributeFormat_e p_format);

    //////////////////////////////////////////////////////////////////////

    // Pointer to the RADIUS packet that this attribute belongs to
    RadiusPacket * m_packet;

    // Offset of the attribute inside the RADIUS packet
    uint16_t m_offset;
};

#endif // _RADIUSATTRIBUTE_H__


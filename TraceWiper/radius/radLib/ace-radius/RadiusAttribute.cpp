/*
 * This source file is part of the ace-radius library.  This code was
 * written by Alex Agranov in 2004-2009, and is covered by the BSD open source
 * license. Refer to the accompanying documentation for details on usage and
 * license.
 */

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#ifdef _MSC_VER
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
#endif
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "RadiusAttribute.h"
#include "RadiusPacket.h"
#include "RadiusSecret.h"
#include "md5.h"


// the following table must be synchronized with definition of Attribute Types in CAttribute.h file
static const char *AttrTypeDescription[] =
{
    "unknown",                  // 0
    "User-Name",                // 1
    "User-Password",            // 2
    "CHAP-Password",            // 3
    "NAS-IP-Address",           // 4
    "NAS-Port",                 // 5
    "Service-Type",             // 6
    "Framed-Protocol",          // 7
    "Framed-IP-Address",        // 8
    "Framed-IP-Netmask",        // 9
    "Framed-Routing",           // 10
    "Filter-Id",                // 11
    "Framed-MTU",               // 12
    "Framed-Compression",       // 13
    "Login-IP-Host",            // 14
    "Login-Service",            // 15
    "Login-TCP-Port",           // 16
    "unknown",                  // 17
    "Reply-Message",            // 18   
    "Callback-Number",          // 19   
    "Callback-Id",              // 20   
    "unknown",                  // 21
    "Framed-Route",             // 22   
    "Framed-IPX-Network",       // 23   
    "State",                    // 24   
    "Class",                    // 25   
    "Vendor-Specific",          // 26   
    "Session-Timeout",          // 27   
    "Idle-Timeout",             // 28   
    "Termination-Action",       // 29   
    "Called-Station-Id",        // 30   
    "Calling-Station-Id",       // 31   
    "NAS-Identifier",           // 32   
    "Proxy-State",              // 33   
    "Login-LAT-Service",        // 34   
    "Login-LAT-Node",           // 35   
    "Login-LAT-Group",          // 36   
    "Framed-AppleTalk-Link",    // 37   
    "Framed-AppleTalk-Network", // 38   
    "Framed-AppleTalk-Zone",    // 39   
    "Acct-Status-Type",         // 40
    "Acct-Delay-Time",          // 41
    "Acct-Input-Octets",        // 42
    "Acct-Output-Octets",       // 43
    "Acct-Session-Id",          // 44
    "Acct-Authentic",           // 45
    "Acct-Session-Time",        // 46
    "Acct-Input-Packets",       // 47
    "Acct-Output-Packets",      // 48
    "Acct-Terminate-Cause",     // 49
    "Acct-Multi-Session-Id",    // 50
    "Acct-Link-Count",          // 51
    "unknown",                  // 52
    "unknown",                  // 53
    "unknown",                  // 54
    "unknown",                  // 55
    "unknown",                  // 56
    "unknown",                  // 57
    "unknown",                  // 58
    "unknown",                  // 59
    "CHAP-Challenge",           // 60   
    "NAS-Port-Type",            // 61   
    "Port-Limit",               // 62   
    "Login-LAT-Port"            // 63   
};


// ---------------------------
// constructor
// ---------------------------
RadiusAttribute::RadiusAttribute() :
    m_offset(0),
    m_packet(NULL)
{
}


// ---------------------------
// destructor
// ---------------------------
RadiusAttribute::~RadiusAttribute()
{
}


// ---------------------------
// get pointer to the attribute raw data
// ---------------------------
unsigned char * RadiusAttribute::getRawData(void)
{
    return m_packet->getRawData() + m_offset + 2;
}


// ---------------------------
// dump attribute in readable text format to the text buffer
// ---------------------------
void RadiusAttribute::dump(AttributeFormat_e p_format)
{
    if (!isValid())
        return;

    // first print attribute type and description
    printf("%2u  %s (%u) = ", getLength(), getTypeDescription(), getType());

    // then print data in human-readable format
    switch (p_format)
    {
    case E_ATTR_FORMAT_INTEGER:
        printf("%u ", getNumber());
        break;
    
    case E_ATTR_FORMAT_IP_ADDRESS:
        {
            struct in_addr l_addr = getIPAddress();
            printf("%s ", inet_ntoa(l_addr));
        }
        break;

    case E_ATTR_FORMAT_STRING:
        {
            const char * l_data;
            uint16_t l_length;
            getString(l_data, l_length);
            
            printf("\"");
            for (int i=0; i<l_length; i++)
            {
                // if the data character is printable - print it
                if ((l_data[i] >= ' ') && (l_data[i] <= 'z'))
                    printf("%c", l_data[i]);
                else
                    printf(".");
            }
            printf("\" ");
        }
        break;
    
    case E_ATTR_FORMAT_USER_PASSWORD:
        {
            char l_data[D_USER_PASSWORD_MAX_LENGTH];
            uint16_t l_length;
            getUserPassword(l_data, l_length);
            
            printf("\"");
            for (int i=0; i<l_length; i++)
            {
                // if the data character is printable - print it
                if ((l_data[i] >= ' ') && (l_data[i] <= 'z'))
                    printf("%c", l_data[i]);
                else
                    printf(".");
            }
            printf("\" ");
        }
        break;
    
    case E_ATTR_FORMAT_CHAP_PASSWORD:
        {
            // TBD
        }
        break;

    case E_ATTR_FORMAT_VENDOR_SPECIFIC:
        {
            const char * l_data;
            uint16_t l_length;
            
            getVendorString(l_data, l_length);
            
            printf("%u \"", getVendorId());
            for (int i=0; i<l_length; i++)
            {
                // if the data character is printable - print it
                if ((l_data[i] >= ' ') && (l_data[i] <= 'z'))
                    printf("%c", l_data[i]);
                else
                    printf(".");
            }
            printf("\" ");
        }
        break;
    }

    // then print data in HEX format
    uint32_t l_length = getLength() - 2;
    unsigned char * l_data = getRawData();

    printf("( ");
    for (uint32_t i=0; i<l_length; i++)
    {
        printf("%02x ", l_data[i]);
    }
    printf(")\n");
}


// ---------------------------
// dump attribute in readable text format to the text buffer
// ---------------------------
void RadiusAttribute::dump()
{
    switch (getType())
    {
    case D_ATTR_NAS_PORT:
    case D_ATTR_SERVICE_TYPE:
    case D_ATTR_FRAMED_PROTOCOL:
    case D_ATTR_FRAMED_ROUTING:
    case D_ATTR_FRAMED_MTU:
    case D_ATTR_FRAMED_COMPRESSION:
    case D_ATTR_LOGIN_SERVICE:
    case D_ATTR_LOGIN_TCP_PORT:
    case D_ATTR_FRAMED_IPX_NETWORK:
    case D_ATTR_SESSION_TIMEOUT:
    case D_ATTR_IDLE_TIMEOUT:
    case D_ATTR_TERMINATION_ACTION:
    case D_ATTR_FRAMED_APPLETALK_LINK:
    case D_ATTR_FRAMED_APPLETALK_NETWORK:
    case D_ATTR_NAS_PORT_TYPE:
    case D_ATTR_PORT_LIMIT:
    case D_ATTR_ACCT_STATUS_TYPE:
    case D_ATTR_ACCT_DELAY_TIME:
    case D_ATTR_ACCT_INPUT_OCTETS:
    case D_ATTR_ACCT_OUTPUT_OCTETS:
    case D_ATTR_ACCT_AUTHENTIC:
    case D_ATTR_ACCT_SESSION_TIME:
    case D_ATTR_ACCT_INPUT_PACKETS:
    case D_ATTR_ACCT_OUTPUT_PACKETS:
    case D_ATTR_ACCT_TERMINATE_CAUSE:
    case D_ATTR_ACCT_LINK_COUNT:

        dump(E_ATTR_FORMAT_INTEGER);
        break;
    case D_ATTR_NAS_IP_ADDRESS:
    case D_ATTR_FRAMED_IP_ADDRESS:
    case D_ATTR_FRAMED_IP_NETMASK:
    case D_ATTR_LOGIN_IP_HOST:
        dump(E_ATTR_FORMAT_IP_ADDRESS);
        break;
    case D_ATTR_USER_PASSWORD:
        dump(E_ATTR_FORMAT_USER_PASSWORD);
        break;
    case D_ATTR_CHAP_PASSWORD:
        dump(E_ATTR_FORMAT_CHAP_PASSWORD);
        break;
    case D_ATTR_VENDOR_SPECIFIC:
        dump(E_ATTR_FORMAT_VENDOR_SPECIFIC);
        break;
    default:
        dump(E_ATTR_FORMAT_STRING);
    }
}



// ---------------------------
// get attribute type
// ---------------------------
unsigned char RadiusAttribute::getType(void)
{
    if (!isValid())
        return 0;

    // A summary of the Attribute format is shown below.  The fields are
    // transmitted from left to right.
    //
    // 0                   1                   2
    // 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    // |     Type      |    Length     |  Value ...
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

    unsigned char * l_data = m_packet->getRawData() + m_offset;
    return *l_data;
}


// ---------------------------
// set attribute type
// ---------------------------
int RadiusAttribute::setType(unsigned char p_type)
{
    if (!isValid())
        return RC_FAIL;

    unsigned char * l_data = m_packet->getRawData() + m_offset;
    *l_data = p_type;
    return RC_SUCCESS;
}


/// get textual description of attribute type
const char * RadiusAttribute::getTypeDescription(void)
{
    unsigned char l_type = getType();
    if ((l_type <= 0) || (l_type > D_ATTR_MAX_KNOWN_TYPE))
        return AttrTypeDescription[0];
    else
        return AttrTypeDescription[l_type];
}


// ---------------------------
// get attribute's length
// ---------------------------
unsigned char RadiusAttribute::getLength(void)
{
    if (!isValid())
        return 0;

    // A summary of the Attribute format is shown below.  The fields are
    // transmitted from left to right.
    //
    // 0                   1                   2
    // 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    // |     Type      |    Length     |  Value ...
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

    unsigned char * l_data = m_packet->getRawData() + m_offset + 1;
    return *l_data;
}


// ---------------------------
// set attribute's length
// ---------------------------
int RadiusAttribute::setLength(unsigned char p_length)
{
    if (!isValid())
        return RC_FAIL;

    unsigned char * l_data = m_packet->getRawData() + m_offset + 1;
    *l_data = p_length;
    return RC_SUCCESS;
}


// ---------------------------
// attribute is valid?
// ---------------------------
int RadiusAttribute::isValid(void)
{
    return (m_offset >= D_RADIUS_PACKET_HEADER_LENGTH);
}


// ---------------------------
// init attribute
// ---------------------------
void RadiusAttribute::init(RadiusPacket * p_packet, uint16_t p_offset)
{
    m_packet = p_packet;
    m_offset = p_offset;
}


// ---------------------------
// get packet that this attribute belongs to
// ---------------------------
RadiusPacket * RadiusAttribute::getPacket(void)
{
    return m_packet;
}


// ---------------------------
// get attribute's offset inside the packet
// ---------------------------
uint16_t RadiusAttribute::getOffset(void)
{
    return m_offset;
}


// ===========================
// Regular attributes
// ===========================

// ---------------------------
// get value of the integer attribute
// ---------------------------
uint32_t RadiusAttribute::getNumber(void)
{
    if (!isValid())
        return 0;
    
    uint32_t l_value;
    memcpy(&l_value, getRawData(), sizeof(uint32_t));
    return ntohl(l_value);
}


// ---------------------------
// set value of the integer attribute
// ---------------------------
int RadiusAttribute::setNumber(uint32_t p_value)
{
    if (!isValid())
        return RC_FAIL;
    
    uint16_t l_length = 6;

    // adjust packet's length
    if (m_packet->adjustLength(l_length) == RC_FAIL)
        return RC_FAIL;

    // encode attribute's length
    setLength((unsigned char)l_length);
    
    // encode attribute's data
    uint32_t l_value = htonl(p_value);
    memcpy(getRawData(), &l_value, sizeof(uint32_t));
    return RC_SUCCESS;
}


// ---------------------------
// get value of the IP address attribute
// ---------------------------
struct in_addr RadiusAttribute::getIPAddress(void)
{
    struct in_addr l_address;

    if (isValid())
#if IN_ADDR_DEEPSTRUCT
        memcpy(&l_address.S_un.S_addr, getRawData(), sizeof(uint32_t));
#else
        memcpy(&l_address, getRawData(), sizeof(uint32_t));
#endif
    
    return l_address;
}


// ---------------------------
// set value of the IP address attribute
// ---------------------------
int RadiusAttribute::setIPAddress(struct in_addr p_address)
{
    if (!isValid())
        return RC_FAIL;
    
    uint16_t l_length = 6;

    // adjust packet's length
    if (m_packet->adjustLength(l_length) == RC_FAIL)
        return RC_FAIL;

    // encode attribute's length
    setLength((unsigned char)l_length);
    
    // encode attribute's data
#if IN_ADDR_DEEPSTRUCT
    memcpy(getRawData(), &p_address.S_un.S_addr, sizeof(uint32_t));
#else
    memcpy(getRawData(), &p_address, sizeof(uint32_t));
#endif
    return RC_SUCCESS;
}


// ---------------------------
// get value of the string attribute
// ---------------------------
int RadiusAttribute::getString(const char * & p_data, uint16_t & p_length)
{
    if (!isValid())
        return RC_FAIL;
    
    p_data = (const char *)getRawData();
    p_length = getLength() - 2;

    return RC_SUCCESS;
}


// ---------------------------
// set value of the string attribute
// ---------------------------
int RadiusAttribute::setString(const char * p_data, uint16_t p_length)
{
    if (!isValid())
        return RC_FAIL;
    
    if (p_data == NULL)
        return RC_FAIL;

    uint16_t l_length;
    if (p_length > 0)
        l_length = p_length + 2;
    else
        l_length = (uint16_t)strlen(p_data) + 2;

    // adjust packet's length
    if (m_packet->adjustLength(l_length) == RC_FAIL)
        return RC_FAIL;

    // encode attribute's length
    setLength((unsigned char)l_length);
    
    // encode attribute's data
    unsigned char * l_data = getRawData();
    memcpy(l_data, p_data, l_length - 2);

    return RC_SUCCESS;
}


// ===========================
// Vendor-Specific attribute
// ===========================
//
//   A summary of the Vendor-Specific Attribute format is shown below.
//   The fields are transmitted from left to right.
//
//   0                   1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |     Type      |  Length       |            Vendor-Id
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//        Vendor-Id (cont)           |  String...
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


// ---------------------------
// get vendor ID of Vendor-Specific attribute
// ---------------------------
uint32_t RadiusAttribute::getVendorId(void)
{
    if (getType() != D_ATTR_VENDOR_SPECIFIC)
        return 0;

    // get vendor Id 
    uint32_t l_value;
    memcpy(&l_value, getRawData(), sizeof(uint32_t));
    return ntohl(l_value);
}


// ---------------------------
// set vendor ID of Vendor-Specific attribute
// ---------------------------
int RadiusAttribute::setVendorId(uint32_t p_vendorId)
{
    if (getType() != D_ATTR_VENDOR_SPECIFIC)
        return RC_FAIL;

    // encode vendor Id 
    uint32_t l_vendorId = htonl(p_vendorId);
    memcpy(getRawData(), &l_vendorId, sizeof(uint32_t));
    return RC_SUCCESS;
}


// ---------------------------
// get "raw" string data of Vendor-Specific attribute
// ---------------------------
int RadiusAttribute::getVendorRawString(const char * & p_data, uint16_t & p_length)
{
    if (!isValid())
        return RC_FAIL;
    
    p_data = (const char *)getRawData() + 4;
    p_length = getLength() - 6;

    return RC_SUCCESS;
}


// ---------------------------
// set "raw" string data of Vendor-Specific attribute
// ---------------------------
int RadiusAttribute::setVendorRawString(const char * p_data, uint16_t p_length)
{
    if (!isValid())
        return RC_FAIL;

    if (p_data == NULL)
        return RC_FAIL;
    
    uint16_t l_length;
    if (p_length > 0)
        l_length = p_length + 6;
    else
        l_length = (uint16_t)strlen(p_data) + 6;

    // adjust packet's length
    if (m_packet->adjustLength(l_length) == RC_FAIL)
        return RC_FAIL;

    // encode attribute's length
    setLength((unsigned char)l_length);
    
    // encode attribute's data
    unsigned char * l_data = getRawData() + 4;
    memcpy(l_data, p_data, l_length - 6);

    return RC_SUCCESS;
}


// ===========================
// User-Password attribute
// ===========================

// ---------------------------
// set string data of User-Password attribute
// ---------------------------
int RadiusAttribute::setUserPassword(const char * p_data, uint16_t p_length)
{
    int l_rc = RC_SUCCESS;

    if (p_data == NULL)
        return RC_FAIL;

    uint16_t l_dataLength;
    if (p_length == 0) 
        l_dataLength = (uint16_t)strlen(p_data);
    else
        l_dataLength = p_length;

    // User-Password attribute's length should be multiple of 16
    uint16_t l_length;
    if (l_dataLength == 0) 
        l_length = 16+2; 
    else 
        l_length = ((l_dataLength + 15) & 0xFFF0) + 2;  

    // adjust packet's length
    if (m_packet->adjustLength(l_length) == RC_FAIL)
        return RC_FAIL;

    // encode attribute's length
    setLength((unsigned char)l_length);

    // if RADIUS packet has secret key properly set, encode UserPassword attribute;
    // otherwise just store the unencrypted data inside the packet - when packet will be
    // sent to the network, it will be re-encoded
    if (m_packet->getSecret() != NULL)
        l_rc = encodeUserPassword((const unsigned char *)p_data, l_dataLength);

    if (l_rc == RC_SUCCESS)
    {
        memcpy(m_packet->m_password, p_data, l_dataLength);
        m_packet->m_passwordLength = l_dataLength;
        m_packet->m_passwordOffset = m_offset;
    }

    return l_rc;
}


// ---------------------------
// encode raw data of User-Password attribute
//
// we need this function in order to re-encode User-Password attribute with new secret key
// when packet is retransmitted through new connection;
//
// ---------------------------
int RadiusAttribute::encodeUserPassword(const unsigned char * p_data, uint16_t p_length)
{
    // local pointer to the raw data buffer
    unsigned char *l_ptr = getRawData();

    // from RFC2865
    //
    //     Call the shared secret S and the pseudo-random 128-bit Request
    //     Authenticator RA.  Break the password into 16-octet chunks p1, p2,
    //     etc.  with the last one padded at the end with nulls to a 16-octet
    //     boundary.  Call the ciphertext blocks c(1), c(2), etc.  We'll need
    //     intermediate values b1, b2, etc.
    //
    //        b1 = MD5(S + RA)       c(1) = p1 xor b1
    //        b2 = MD5(S + c(1))     c(2) = p2 xor b2
    //               .                       .
    //               .                       .
    //               .                       .
    //        bi = MD5(S + c(i-1))   c(i) = pi xor bi
    //
    //     The String will contain c(1)+c(2)+...+c(i) where + denotes
    //     concatenation.

    int i;
    uint16_t l_chunkOffset, l_chunkLength;
    unsigned char b[16], c[16], p[16], md5Buf[D_SECRET_MAX_LENGTH+16];
    
    // init c(0) with pseudo-random RA
    memcpy(c, m_packet->getAuthenticator(), 16);

    // copy shared secret to the beginning of md5Buf
    const unsigned char * l_secret;
    uint16_t l_secretLength;

    RadiusSecret * l_secretKey = m_packet->getSecret();
    if (l_secretKey != NULL)
        l_secretKey->getSecret(l_secret, l_secretLength);
    else
        l_secretLength = 0;

    if (l_secretLength == 0)
        return RC_FAIL;

    memcpy(md5Buf, l_secret, l_secretLength);

    for (l_chunkOffset=0; l_chunkOffset<p_length; l_chunkOffset += 16)
    {
        // calculate p(i)
        // (pad password to the 16-octet boundary with NULLs)
        l_chunkLength = p_length-l_chunkOffset;
        if (l_chunkLength >= 16)
            memcpy(p, p_data+l_chunkOffset, 16);
        else
        {
            memcpy(p, p_data+l_chunkOffset, l_chunkLength);
            memset(p+l_chunkLength, 0, 16-l_chunkLength);
        }

        // calculate b(i) = MD5(S + c(i-1))
        memcpy(md5Buf + l_secretLength, c, 16);
        MD5Calc(b, md5Buf, l_secretLength+16);

        // calculate c(i) = p(i) xor b(i)
        for (i=0; i<16; i++)
        {
            c[i] = p[i] ^ b[i];
        }
        
        // copy c(i) to the attribute value
        memcpy(l_ptr, c, 16);
        l_ptr += 16;
    }
    
    return RC_SUCCESS;
}


// ---------------------------
// get string data of User-Password attribute
// ---------------------------
int RadiusAttribute::getUserPassword(char * p_data, uint16_t & p_length, RadiusSecret * p_secret)
{
    // from RFC2865
    //
    //     Call the shared secret S and the pseudo-random 128-bit Request
    //     Authenticator RA.  Break the password into 16-octet chunks p1, p2,
    //     etc.  with the last one padded at the end with nulls to a 16-octet
    //     boundary.  Call the ciphertext blocks c(1), c(2), etc.  We'll need
    //     intermediate values b1, b2, etc.
    //
    //        b1 = MD5(S + RA)       c(1) = p1 xor b1
    //        b2 = MD5(S + c(1))     c(2) = p2 xor b2
    //               .                       .
    //               .                       .
    //               .                       .
    //        bi = MD5(S + c(i-1))   c(i) = pi xor bi
    //
    //     The String will contain c(1)+c(2)+...+c(i) where + denotes
    //     concatenation.
    //
    //     On receipt, the process is reversed to yield the original
    //     password.

    // local pointer to the raw data buffer
    unsigned char *l_data = getRawData();

    // length of the password attribute
    p_length = getLength() - 2;

    int i;
    uint16_t l_chunkOffset;
    unsigned char b[16], c[16], p[16], md5Buf[D_SECRET_MAX_LENGTH+16];
    
    // local buffer for the decoded password
    char *l_ptr = p_data;

    // init c(0) with pseudo-random RA
    memcpy(c, m_packet->getAuthenticator(), 16);

    // copy shared secret to the beginning of md5Buf
    const unsigned char * l_secret;
    uint16_t l_secretLength;
    
    if (p_secret != NULL)
        p_secret->getSecret(l_secret, l_secretLength);
    else
    {
        RadiusSecret * l_secretKey = m_packet->getSecret();
        if (l_secretKey != NULL)
            l_secretKey->getSecret(l_secret, l_secretLength);
        else
            l_secretLength = 0;
    }
    
    if (l_secretLength == 0)
    {
        if (m_packet->m_passwordLength > 0)
        {
            p_length = m_packet->m_passwordLength;
            memcpy(p_data, m_packet->m_password, p_length);
            return RC_SUCCESS;
        }
        else
            return RC_FAIL;
    }
        
    memcpy(md5Buf, l_secret, l_secretLength);

    for (l_chunkOffset=0; l_chunkOffset<p_length; l_chunkOffset += 16)
    {
        // calculate b(i) = MD5(S + c(i-1))
        memcpy(md5Buf + l_secretLength, c, 16);
        MD5Calc(b, md5Buf, l_secretLength+16);

        // calculate c(i)
        memcpy(c, l_data+l_chunkOffset, 16);

        // calculate p(i) = c(i) xor b(i)
        for (i=0; i<16; i++)
        {
            p[i] = c[i] ^ b[i];
        }
        
        // copy p(i) to the password value
        memcpy(l_ptr, p, 16);
        l_ptr += 16;
    }
    
    // decoded password length - remove padded NULLs
    while ((p_data[p_length-1] == '\0') && (p_length > 0))
    {
        --p_length;
    }

    return RC_SUCCESS;
}


// ===========================
// Old-Password attribute
// ===========================

// ---------------------------
// set string data of Old-Password attribute
// ---------------------------
int RadiusAttribute::setOldPassword(const char * p_data, uint16_t p_length)
{
    int l_rc = RC_SUCCESS;

    if (p_data == NULL)
        return RC_FAIL;

    uint16_t l_dataLength;
    if (p_length == 0) 
        l_dataLength = (uint16_t)strlen(p_data);
    else
        l_dataLength = p_length;

    // Old-Password attribute's length should be multiple of 16
    uint16_t l_length;
    if (l_dataLength == 0) 
        l_length = 16+2; 
    else 
        l_length = ((l_dataLength + 15) & 0xFFF0) + 2;  

    // adjust packet's length
    if (m_packet->adjustLength(l_length) == RC_FAIL)
        return RC_FAIL;

    // encode attribute's length
    setLength((unsigned char)l_length);

    // if RADIUS packet has secret key properly set, encode UserPassword attribute;
    // otherwise just store the unencrypted data inside the packet - when packet will be
    // sent to the network, it will be re-encoded
    if (m_packet->getSecret() != NULL)
        l_rc = encodeOldPassword((const unsigned char *)p_data, l_dataLength);

    if (l_rc == RC_SUCCESS)
    {
        memcpy(m_packet->m_oldPassword, p_data, l_dataLength);
        m_packet->m_oldPasswordLength = l_dataLength;
        m_packet->m_oldPasswordOffset = m_offset;
    }

    return l_rc;
}


// ---------------------------
// encode raw data of Old-Password attribute
//
// we need this function in order to re-encode User-Password attribute with new secret key
// when packet is retransmitted through new connection;
//
// ---------------------------
int RadiusAttribute::encodeOldPassword(const unsigned char * p_data, uint16_t p_length)
{
    // local pointer to the raw data buffer
    unsigned char *l_ptr = getRawData();

    // from RFC2865
    //
    //     Call the shared secret S and the pseudo-random 128-bit Request
    //     Authenticator RA.  Break the password into 16-octet chunks p1, p2,
    //     etc.  with the last one padded at the end with nulls to a 16-octet
    //     boundary.  Call the ciphertext blocks c(1), c(2), etc.  We'll need
    //     intermediate values b1, b2, etc.
    //
    //        b1 = MD5(S + RA)       c(1) = p1 xor b1
    //        b2 = MD5(S + c(1))     c(2) = p2 xor b2
    //               .                       .
    //               .                       .
    //               .                       .
    //        bi = MD5(S + c(i-1))   c(i) = pi xor bi
    //
    //     The String will contain c(1)+c(2)+...+c(i) where + denotes
    //     concatenation.

    int i;
    uint16_t l_chunkOffset, l_chunkLength;
    unsigned char b[16], c[16], p[16], md5Buf[D_SECRET_MAX_LENGTH+16];
    
    // init c(0) with pseudo-random RA
    memcpy(c, m_packet->getAuthenticator(), 16);

    // for Old-Password attribute we should use encoded value of User-Password 
    // attribute instead of  Request Authenticator (this is some legacy 
    // standard that is still supported by many servers)
    RadiusAttribute l_attr;
    if (m_packet->findAttribute(D_ATTR_OLD_PASSWORD, l_attr) == RC_SUCCESS)
        memcpy(c, l_attr.getRawData(), 16);

    // copy shared secret to the beginning of md5Buf
    const unsigned char * l_secret;
    uint16_t l_secretLength;

    RadiusSecret * l_secretKey = m_packet->getSecret();
    if (l_secretKey != NULL)
        l_secretKey->getSecret(l_secret, l_secretLength);
    else
        l_secretLength = 0;

    if (l_secretLength == 0)
        return RC_FAIL;

    memcpy(md5Buf, l_secret, l_secretLength);

    for (l_chunkOffset=0; l_chunkOffset<p_length; l_chunkOffset += 16)
    {
        // calculate p(i)
        // (pad password to the 16-octet boundary with NULLs)
        l_chunkLength = p_length-l_chunkOffset;
        if (l_chunkLength >= 16)
            memcpy(p, p_data+l_chunkOffset, 16);
        else
        {
            memcpy(p, p_data+l_chunkOffset, l_chunkLength);
            memset(p+l_chunkLength, 0, 16-l_chunkLength);
        }

        // calculate b(i) = MD5(S + c(i-1))
        memcpy(md5Buf + l_secretLength, c, 16);
        MD5Calc(b, md5Buf, l_secretLength+16);

        // calculate c(i) = p(i) xor b(i)
        for (i=0; i<16; i++)
        {
            c[i] = p[i] ^ b[i];
        }
        
        // copy c(i) to the attribute value
        memcpy(l_ptr, c, 16);
        l_ptr += 16;
    }
    
    return RC_SUCCESS;
}


// ---------------------------
// get string data of User-Password attribute
// ---------------------------
int RadiusAttribute::getOldPassword(char * p_data, uint16_t & p_length, RadiusSecret * p_secret)
{
    // from RFC2865
    //
    //     Call the shared secret S and the pseudo-random 128-bit Request
    //     Authenticator RA.  Break the password into 16-octet chunks p1, p2,
    //     etc.  with the last one padded at the end with nulls to a 16-octet
    //     boundary.  Call the ciphertext blocks c(1), c(2), etc.  We'll need
    //     intermediate values b1, b2, etc.
    //
    //        b1 = MD5(S + RA)       c(1) = p1 xor b1
    //        b2 = MD5(S + c(1))     c(2) = p2 xor b2
    //               .                       .
    //               .                       .
    //               .                       .
    //        bi = MD5(S + c(i-1))   c(i) = pi xor bi
    //
    //     The String will contain c(1)+c(2)+...+c(i) where + denotes
    //     concatenation.
    //
    //     On receipt, the process is reversed to yield the original
    //     password.

    // local pointer to the raw data buffer
    unsigned char *l_data = getRawData();

    // length of the password attribute
    p_length = getLength() - 2;

    int i;
    uint16_t l_chunkOffset;
    unsigned char b[16], c[16], p[16], md5Buf[D_SECRET_MAX_LENGTH+16];
    
    // local buffer for the decoded password
    char *l_ptr = p_data;

    // init c(0) with pseudo-random RA
    memcpy(c, m_packet->getAuthenticator(), 16);

    // for Old-Password attribute we should use encoded value of User-Password 
    // attribute instead of  Request Authenticator (this is some legacy 
    // standard that is still supported by many servers)
    RadiusAttribute l_attr;
    if (m_packet->findAttribute(D_ATTR_OLD_PASSWORD, l_attr) == RC_SUCCESS)
        memcpy(c, l_attr.getRawData(), 16);

    // copy shared secret to the beginning of md5Buf
    const unsigned char * l_secret;
    uint16_t l_secretLength;
    
    if (p_secret != NULL)
        p_secret->getSecret(l_secret, l_secretLength);
    else
    {
        RadiusSecret * l_secretKey = m_packet->getSecret();
        if (l_secretKey != NULL)
            l_secretKey->getSecret(l_secret, l_secretLength);
        else
            l_secretLength = 0;
    }
    
    if (l_secretLength == 0)
    {
        if (m_packet->m_oldPasswordLength > 0)
        {
            p_length = m_packet->m_oldPasswordLength;
            memcpy(p_data, m_packet->m_oldPassword, p_length);
            return RC_SUCCESS;
        }
        else
            return RC_FAIL;
    }
        
    memcpy(md5Buf, l_secret, l_secretLength);

    for (l_chunkOffset=0; l_chunkOffset<p_length; l_chunkOffset += 16)
    {
        // calculate b(i) = MD5(S + c(i-1))
        memcpy(md5Buf + l_secretLength, c, 16);
        MD5Calc(b, md5Buf, l_secretLength+16);

        // calculate c(i)
        memcpy(c, l_data+l_chunkOffset, 16);

        // calculate p(i) = c(i) xor b(i)
        for (i=0; i<16; i++)
        {
            p[i] = c[i] ^ b[i];
        }
        
        // copy p(i) to the password value
        memcpy(l_ptr, p, 16);
        l_ptr += 16;
    }
    
    // decoded password length - remove padded NULLs
    while ((p_data[p_length-1] == '\0') && (p_length > 0))
    {
        --p_length;
    }

    return RC_SUCCESS;
}


// ===========================
// CHAP-Password attribute
// ===========================
//
//   A summary of the CHAP-Password Attribute format is shown below.  The
//   fields are transmitted from left to right.
//
//    0                   1                   2
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
//   |     Type      |    Length     |  CHAP Ident   |  String ...
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


// ---------------------------
// get CHAP identifier of CHAP-Password attribute
// ---------------------------
unsigned char RadiusAttribute::getChapIdentifier(void)
{
    unsigned char * l_data = getRawData();
    return *l_data;
}

    
// ---------------------------
// set vendor ID of CHAP-Password attribute
// ---------------------------
int RadiusAttribute::setChapIdentifier(unsigned char p_chapId)
{
    unsigned char * l_data = getRawData();
    *l_data = p_chapId;
    return RC_SUCCESS;
}


// ---------------------------
// get string data of CHAP-Password attribute
// ---------------------------
int RadiusAttribute::getChapString(const char * & p_data, uint16_t & p_length)
{
    if (!isValid())
        return RC_FAIL;
    
    p_data = (const char *)getRawData() + 1;
    p_length = getLength() - 3;

    return RC_SUCCESS;
}
   
// ---------------------------
// set string data of CHAP-Password attribute
// ---------------------------
int RadiusAttribute::setChapString(const char * p_data, uint16_t p_length)
{
    if (!isValid())
        return RC_FAIL;
    
    if (p_data == NULL)
        return RC_FAIL;

    uint16_t l_length;
    if (p_length > 0)
        l_length = p_length + 3;
    else
        l_length = (uint16_t)strlen(p_data) + 3;

    // adjust packet's length
    if (m_packet->adjustLength(l_length) == RC_FAIL)
        return RC_FAIL;

    // encode attribute's length
    setLength((unsigned char)l_length);
    
    // encode attribute's data
    unsigned char * l_data = getRawData() + 1;
    memcpy(l_data, p_data, l_length - 3);

    return RC_SUCCESS;
}



/*
    Additional access methods for Vendor-Specific attributes

       0                   1                   2                   3
       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |     Type      |  Length       |            Vendor-Id
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
           Vendor-Id (cont)           | Vendor type   | Vendor length |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |    Attribute-Specific...
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
*/


// ---------------------------
// get Vendor Type field
// ---------------------------
unsigned char RadiusAttribute::getVendorType()
{
    if (!isValid())
        return 0;

    unsigned char * l_ptr = getRawData() + 4;
    return *l_ptr;
}



// ---------------------------
// set Vendor Type field
// ---------------------------
int RadiusAttribute::setVendorType(unsigned char p_value)
{
    if (!isValid())
        return RC_FAIL;

    unsigned char * l_ptr = getRawData() + 4;
    *l_ptr = p_value;

    return RC_SUCCESS;
}


// ---------------------------
// get Vendor Length field
// ---------------------------
unsigned char RadiusAttribute::getVendorLength()
{
    if (!isValid())
        return 0;

    unsigned char * l_ptr = getRawData() + 5;
    return *l_ptr;
}


// ---------------------------
// set Vendor Length field
// ---------------------------
int RadiusAttribute::setVendorLength(unsigned char p_length)
{
    if (!isValid())
        return RC_FAIL;

    unsigned char * l_ptr = getRawData() + 5;
    *l_ptr = p_length;

    return RC_SUCCESS;
}


// ---------------------------
// get value of the integer Vendor-Specific attribute
// ---------------------------
uint32_t RadiusAttribute::getVendorNumber()
{
    if (!isValid())
        return 0;
    
    uint32_t l_value;
    memcpy(&l_value, getRawData()+6, sizeof(uint32_t));
    return ntohl(l_value);
}


// ---------------------------
// set value of the integer Vendor-Specific attribute
// ---------------------------
int RadiusAttribute::setVendorNumber(uint32_t p_value)
{
    if (!isValid())
        return RC_FAIL;
    
    uint16_t l_length = 12;

    // adjust packet's length
    if (m_packet->adjustLength(l_length) == RC_FAIL)
        return RC_FAIL;

    // encode attribute's length
    setLength((unsigned char)l_length);
    setVendorLength(l_length - 6);

    // encode attribute's data
    uint32_t l_value = htonl(p_value);
    memcpy(getRawData()+ 6, &l_value, sizeof(uint32_t));
    return RC_SUCCESS;

}


// ---------------------------
// get value of the string Vendor-Specific attribute
// ---------------------------
int RadiusAttribute::getVendorString(const char * & p_data, uint16_t & p_length)
{
    if (!isValid())
        return RC_FAIL;
    
    p_data = (const char *)getRawData() + 6;
    p_length = getLength() - 8;

    return RC_SUCCESS;
}


// ---------------------------
// set value of the string Vendor-Specific attribute
// ---------------------------
int RadiusAttribute::setVendorString(const char * p_data, uint16_t p_length)
{
    if (!isValid())
        return RC_FAIL;
    
    if (p_data == NULL)
        return RC_FAIL;

    uint16_t l_length;
    if (p_length > 0)
        l_length = p_length + 8;
    else
        l_length = (uint16_t)strlen(p_data) + 8;

    // adjust packet's length
    if (m_packet->adjustLength(l_length) == RC_FAIL)
        return RC_FAIL;

    // encode attribute's length
    setLength((unsigned char)l_length);
    setVendorLength((unsigned char)l_length - 6);

    // encode attribute's data
    unsigned char * l_data = getRawData() + 6;
    memcpy(l_data, p_data, l_length - 8);

    return RC_SUCCESS;

}


// ---------------------------
// get value of the IP address Vendor-Specific attribute
// ---------------------------
struct in_addr RadiusAttribute::getVendorIPAddress()
{
    struct in_addr l_address;

    if (isValid())
#if IN_ADDR_DEEPSTRUCT
        memcpy(&l_address.S_un.S_addr, getRawData()+6, sizeof(uint32_t));
#else
        memcpy(&l_address, getRawData()+6, sizeof(uint32_t));
#endif
    
    return l_address;
}


// ---------------------------
// set value of the IP address Vendor-Specific attribute
// ---------------------------
int RadiusAttribute::setVendorIPAddress(struct in_addr p_address)   
{
    if (!isValid())
        return RC_FAIL;
    
    uint16_t l_length = 12;

    // adjust packet's length
    if (m_packet->adjustLength(l_length) == RC_FAIL)
        return RC_FAIL;

    // encode attribute's length
    setLength((unsigned char)l_length);
    setVendorLength((unsigned char)l_length - 6);
    
    // encode attribute's data
#if IN_ADDR_DEEPSTRUCT
    memcpy(getRawData()+6, &p_address.S_un.S_addr, sizeof(uint32_t));
#else
    memcpy(getRawData()+6, &p_address, sizeof(uint32_t));
#endif
	return RC_SUCCESS;
}


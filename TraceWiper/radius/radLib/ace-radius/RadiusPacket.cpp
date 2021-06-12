/*
 * This source file is part of the ace-radius library.  This code was
 * written by Alex Agranov in 2004-2009, and is covered by the BSD open source
 * license. Refer to the accompanying documentation for details on usage and
 * license.
 */

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#ifndef _MSC_VER
    #include <unistd.h>
    #include <sys/time.h>
    #include <arpa/inet.h>
#endif
#include <string.h>
#include <stdio.h>
#include "afx.h"
#include "RadiusPacket.h"
#include "RadiusSecret.h"
#include "RadiusClientStack.h"

#include "Radius.h"
#include "md5.h"



// the following table must be synchronized with definition of Packet Codes in RadiusPacket.h file
static const char *PacketCodeDescription[] =
{
    "unknown",                  // 0
    "Access-Request",           // 1
    "Access-Accept",            // 2
    "Access-Reject",            // 3
    "Accounting-Request",       // 4
    "Accounting-Response",      // 5
    "unknown",                  // 6
    "unknown",                  // 7
    "unknown",                  // 8
    "unknown",                  // 9
    "unknown",                  // 10
    "Access-Challenge",         // 11
    "Status-Server",            // 12
    "Status-Client",            // 13
};


// ---------------------------
// Constructor
// ---------------------------
RadiusPacket::RadiusPacket(unsigned char p_code)
{
    init(p_code);
    randomAuthenticator();
}


// ---------------------------
// Constructor
// ---------------------------
RadiusPacket::RadiusPacket(unsigned char p_code, RadiusClientStack & p_stack)
{
    init(p_code);
    randomAuthenticator();
    setID(p_stack.getNextPacketID());
}

// ---------------------------
// Constructor
// ---------------------------
RadiusPacket::RadiusPacket(unsigned char p_code, RadiusPacket & p_request)
{
    init(p_code);
    setID(p_request.getID());
    m_secret = p_request.getSecret();
    setAuthenticator(p_request.getAuthenticator());
}


// ---------------------------
// Init packet
// ---------------------------
void RadiusPacket::init(unsigned char p_code)
{
    setCode(p_code);
    setLength(D_RADIUS_PACKET_HEADER_LENGTH);
    setID(0);

    m_secret = NULL;

    m_passwordLength = 0;
    m_passwordOffset = 0;

    m_oldPasswordLength = 0;
    m_oldPasswordOffset = 0;
}

// ---------------------------
// Copy constructor
// ---------------------------
RadiusPacket::RadiusPacket(const RadiusPacket & p_packet)
{
    memcpy(m_data, p_packet.m_data, D_RAW_DATA_LENGTH);
    m_secret = p_packet.m_secret;

    m_passwordLength = p_packet.m_passwordLength;
    m_passwordOffset = p_packet.m_passwordOffset;
    if (m_passwordLength > 0)
        memcpy(m_password, p_packet.m_password, m_passwordLength);

    m_oldPasswordLength = p_packet.m_oldPasswordLength;
    m_oldPasswordOffset = p_packet.m_oldPasswordOffset;
    if (m_oldPasswordLength > 0)
        memcpy(m_oldPassword, p_packet.m_oldPassword, m_oldPasswordLength);
}


// ---------------------------
// Destructor
// ---------------------------
RadiusPacket::~RadiusPacket()
{
}


// ---------------------------
// Get packet code
// ---------------------------
unsigned char RadiusPacket::getCode(void)
{
    return m_data[0];
}


// ---------------------------
// Set packet code
// ---------------------------
void RadiusPacket::setCode(unsigned char p_code)
{
    m_data[0] = p_code;
}


// ---------------------------
// Get packet code description
// ---------------------------
const char * RadiusPacket::getCodeDescription(void)
{
    unsigned char l_code = getCode();
    if (l_code > D_PACKET_MAX_KNOWN_CODE)
        return PacketCodeDescription[0];
    else
        return PacketCodeDescription[l_code];
}



// ---------------------------
// Get packet identificator
// ---------------------------
unsigned char RadiusPacket::getID(void)
{
    return m_data[1];
}


// ---------------------------
// Set packet code
// ---------------------------
void RadiusPacket::setID(unsigned char p_id)
{
    m_data[1] = p_id;
}


// ---------------------------
// Set random packet identifier
// ---------------------------
void RadiusPacket::randomID()
{
#ifdef _MSC_VER
    SYSTEMTIME l_time;
    GetSystemTime(&l_time);

    unsigned char l_id = l_time.wMilliseconds & 0xFF;
#else
    struct timeval  l_time;
    struct timezone l_zone;
    gettimeofday(&l_time,&l_zone);

    unsigned char l_id = (l_time.tv_usec / 1000) & 0xFF;
#endif
    m_data[1] = l_id;
}


// ---------------------------
// Set packet code
// ---------------------------
void RadiusPacket::setSecret(RadiusSecret * p_secret)
{
    m_secret = p_secret;
}


// ---------------------------
// Get packet's length
// ---------------------------
uint16_t RadiusPacket::getLength(void)
{
    uint16_t l_length;
    memcpy(&l_length, m_data + 2, sizeof(uint16_t));
    return (ntohs(l_length));
}


// ---------------------------
// Set packet's length
// ---------------------------
void RadiusPacket::setLength(uint16_t p_length)
{
    uint16_t l_length = htons(p_length);
    memcpy(m_data + 2, &l_length, sizeof(uint16_t));
}



// ---------------------------
// Get packet authenticator
// ---------------------------
const unsigned char * RadiusPacket::getAuthenticator(void)
{
    return m_data + 4;
}


// ---------------------------
// Set packet authenticator
// ---------------------------
void RadiusPacket::setAuthenticator(const unsigned char * p_auth)
{
    memcpy(m_data + 4, p_auth, D_AUTHENTICATOR_LENGTH);
}


// ---------------------------
// Get secret key associated with this packet
// ---------------------------
RadiusSecret * RadiusPacket::getSecret(void)
{
    return m_secret;
}


// ---------------------------
// Generate pseudo-random vector
// ---------------------------
void RadiusPacket::randomAuthenticator(void)
{
    char l_hash[256], *l_ptr = l_hash;
    gethostname(l_ptr, sizeof(l_hash)-50);
    l_ptr += strlen(l_ptr);

#ifdef _MSC_VER
    SYSTEMTIME l_time;
    GetSystemTime(&l_time);
    //sprintf_s(l_ptr, 50, ":%u:%u", l_time.wSecond, l_time.wMilliseconds);
    sprintf(l_ptr, /*50,*/ ":%u:%u", l_time.wSecond, l_time.wMilliseconds);
#else
    struct timeval  l_time;
    struct timezone l_zone;
    gettimeofday(&l_time,&l_zone);
    sprintf(l_ptr, ":%u:%u", l_time.tv_sec, l_time.tv_usec);
#endif
    l_ptr += strlen(l_ptr);

    MD5Calc(m_data + 4, (unsigned char *)l_hash, (uint32_t)strlen(l_hash));

}


// ---------------------------
// dump radius packet in readable text format to the STDOUT
// ---------------------------
void RadiusPacket::dump()
{
    int i;

    // first dump packet raw data in HEX format
    uint16_t l_length = getLength();
    for (i=0; i<l_length; i++)
    {
        if (i%16 == 0)
            TRACE("\n");
        TRACE("%02x ", m_data[i]);
    }

    TRACE("\n\n");

    // dump packet code and code description
    TRACE(" 1  Code = %s (%u)\n", getCodeDescription(), getCode());
    
    // dump packet identifier
    TRACE(" 1  ID = %u\n", getID());
    
    // dump packet length
    TRACE(" 2  Length = %u\n", getLength());

    // dump request authenticator
    TRACE("16  Request Authenticator = ( ");

    const unsigned char * l_auth = getAuthenticator();
    for (i=0; i<D_AUTHENTICATOR_LENGTH; i++)
    {
        TRACE("%02x ", *l_auth++);
    }

    TRACE(")\n");

    
    RadiusAttribute l_attr;
    for (getFirstAttribute(l_attr);
         l_attr.isValid();
         getNextAttribute(l_attr))
    {
        l_attr.dump();
    }

    TRACE("\n");
}



// ---------------------------
// Get first attribute in the packet
// ---------------------------
int RadiusPacket::getFirstAttribute(RadiusAttribute & p_attr)
{
    if (getLength() == D_RADIUS_PACKET_HEADER_LENGTH)
    {
        p_attr.init(this, 0);
        return RC_FAIL;
    }
    
    p_attr.init(this, D_RADIUS_PACKET_HEADER_LENGTH);
    return RC_SUCCESS;
}


// ---------------------------
// Get next attribute in the packet
// ---------------------------
int RadiusPacket::getNextAttribute(RadiusAttribute & p_attr)
{
    unsigned int l_offset = p_attr.getOffset(),
                 l_length = p_attr.getLength();
    
    if ((l_length == 0) || 
        (l_offset + l_length >= getLength()))
    {
        p_attr.init(this, 0);
        return RC_FAIL;
    }

    p_attr.init(this, l_offset + l_length);
    return RC_SUCCESS;
}


// ---------------------------
// Find specific attribute in the packet
// ---------------------------
int RadiusPacket::findAttribute(unsigned char p_attrType, RadiusAttribute & p_attr)
{
    for(getFirstAttribute(p_attr); p_attr.isValid(); getNextAttribute(p_attr))
    {
        if (p_attr.getType() == p_attrType)
            return RC_SUCCESS;
    }
    
    return RC_FAIL;
}


// ---------------------------
// Add new attribute to the packet
// ---------------------------
int RadiusPacket::addAttribute(unsigned char p_attrType, RadiusAttribute & p_attr)
{
    uint16_t l_length = getLength();
    if (l_length >= D_RADIUS_PACKET_MAX_LENGTH - 1)
    {
        p_attr.init(this, 0);
        return RC_FAIL;
    }

    p_attr.init(this, l_length);
    p_attr.setType(p_attrType);
    return RC_SUCCESS;
}


// ---------------------------
// Get raw packet data
// ---------------------------
unsigned char * RadiusPacket::getRawData(void)
{
    return m_data;
}


// ---------------------------
// adjust packet's length
// ---------------------------
int RadiusPacket::adjustLength(uint16_t p_length)
{
    uint16_t l_length = getLength();
    if (l_length + p_length > D_RADIUS_PACKET_MAX_LENGTH)
        return RC_FAIL;

    setLength(l_length + p_length);
    return RC_SUCCESS;
}


// ---------------------------
// Generate authenticator for response packet
// ---------------------------
void RadiusPacket::responseAuthenticator(RadiusSecret * p_secret)
{
    // from RFC2865:
    //
    //   Response Authenticator
    //
    //      The value of the Authenticator field in Access-Accept, Access-
    //      Reject, and Access-Challenge packets is called the Response
    //      Authenticator, and contains a one-way MD5 hash calculated over
    //      a stream of octets consisting of: the RADIUS packet, beginning
    //      with the Code field, including the Identifier, the Length, the
    //      Request Authenticator field from the Access-Request packet, and
    //      the response Attributes, followed by the shared secret.  That
    //      is, ResponseAuth =
    //      MD5(Code+ID+Length+RequestAuth+Attributes+Secret) where +
    //      denotes concatenation.
    //

    const unsigned char * l_secret;
    uint16_t l_secretLength;

    if (p_secret != NULL)
        p_secret->getSecret(l_secret, l_secretLength);
    else
        m_secret->getSecret(l_secret, l_secretLength);
        
    memcpy(m_data + getLength(), l_secret, l_secretLength);

    unsigned char l_authenticator[D_AUTHENTICATOR_LENGTH];
    MD5Calc(l_authenticator, m_data, getLength() + l_secretLength);

    setAuthenticator(l_authenticator);
}


// ---------------------------
// Encode User-Password attribute
// ---------------------------
void RadiusPacket::encodeUserPassword(void)
{
    if (m_passwordLength > 0)
    {
        RadiusAttribute l_attr;
        l_attr.init(this, m_passwordOffset);
        l_attr.encodeUserPassword(m_password, m_passwordLength);
    }

    if (m_oldPasswordLength > 0)
    {
        RadiusAttribute l_attr;
        l_attr.init(this, m_oldPasswordOffset);
        l_attr.encodeOldPassword(m_oldPassword, m_oldPasswordLength);
    }
}


// ---------------------------
// Generate authenticator for accounting request packet
// ---------------------------
void RadiusPacket::accountingRequestAuthenticator(RadiusSecret * p_secret) 
{
    // from RFC2866:
    //
    //   Request Authenticator
    //
    //     In Accounting-Request Packets, the Authenticator value is a 16
    //     octet MD5 [5] checksum, called the Request Authenticator.
    //
    //     The NAS and RADIUS accounting server share a secret.  The Request
    //     Authenticator field in Accounting-Request packets contains a one-
    //     way MD5 hash calculated over a stream of octets consisting of the
    //     Code + Identifier + Length + 16 zero octets + request attributes +
    //     shared secret (where + indicates concatenation).

    const unsigned char * l_secret;
    uint16_t l_secretLength;

    if (p_secret != NULL)
        p_secret->getSecret(l_secret, l_secretLength);
    else
        m_secret->getSecret(l_secret, l_secretLength);

    memcpy(m_data + getLength(), l_secret, l_secretLength);

    unsigned char l_authenticator[D_AUTHENTICATOR_LENGTH];
    memset (l_authenticator, 0, D_AUTHENTICATOR_LENGTH);
    setAuthenticator (l_authenticator);

    MD5Calc(l_authenticator, m_data, getLength() + l_secretLength);

    setAuthenticator(l_authenticator);
}


/// Verify RADIUS response
bool RadiusPacket::verifyResponse(RadiusPacket & p_packet)
{
    const unsigned char * l_secret;
    uint16_t l_secretLength;

    m_secret->getSecret(l_secret, l_secretLength);

    // for password change, old_password must be used instead of the original secret
    if (m_oldPasswordLength > 0)
    {
        l_secret = m_oldPassword;
        l_secretLength = m_oldPasswordLength;
    }

    // store original reply authenticator
    unsigned char l_responseAuthenticator[D_AUTHENTICATOR_LENGTH];
    memcpy(l_responseAuthenticator, p_packet.getAuthenticator(), D_AUTHENTICATOR_LENGTH);

    // put request authenticator in place
    p_packet.setAuthenticator(getAuthenticator());
    // append shared secret 
    memcpy(p_packet.getRawData() + p_packet.getLength(), l_secret, l_secretLength);

    // encode reply authenticator (as server was supposed to do)
    unsigned char l_authenticator[D_AUTHENTICATOR_LENGTH];
    MD5Calc(l_authenticator, p_packet.getRawData(), p_packet.getLength() + l_secretLength);

    // restore original response authenticator
    p_packet.setAuthenticator(l_responseAuthenticator);

    // compare the result with the received authenticator
    if (memcmp(l_authenticator, l_responseAuthenticator, D_AUTHENTICATOR_LENGTH) != 0)
        return false;

    if (getID() != p_packet.getID())
        return false;

    return true;
}




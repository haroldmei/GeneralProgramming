/*
 * This source file is part of the ace-radius library.  This code was
 * written by Alex Agranov in 2004-2009, and is covered by the BSD open source
 * license. Refer to the accompanying documentation for details on usage and
 * license.
 */

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <string.h>

#include "RadiusSecret.h"


// ---------------------------
// constructor
// ---------------------------
RadiusSecret::RadiusSecret() :
    m_secretLength(0)
{
}


// ---------------------------
// constructor
// ---------------------------
RadiusSecret::RadiusSecret(const char * p_string)
{
    m_secretLength = (uint16_t)strlen(p_string);
    memcpy(m_secret, p_string, m_secretLength);
}


// ---------------------------
// Copy constructor
// ---------------------------
RadiusSecret::RadiusSecret(const RadiusSecret & other)
{
    m_secretLength = other.m_secretLength;
    memcpy(m_secret, other.m_secret, m_secretLength);
}


// ---------------------------
// Get secret key
// ---------------------------
int RadiusSecret::getSecret(const unsigned char * & p_data, uint16_t & p_length)
{
    if (m_secretLength == 0)
        return RC_FAIL;
    
    p_data = m_secret;
    p_length = m_secretLength;
    return RC_SUCCESS;
}


// ---------------------------
// Set secret key
// ---------------------------
int RadiusSecret::setSecret(const unsigned char * p_data, uint16_t p_length)
{
    if (p_length > D_SECRET_MAX_LENGTH)
        return RC_FAIL;

    memcpy(m_secret, p_data, p_length);
    m_secretLength = p_length;
    return RC_SUCCESS;
}



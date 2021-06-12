/*
 * This source file is part of the ace-radius library.  This code was
 * written by Alex Agranov in 2004-2009, and is covered by the BSD open source
 * license. Refer to the accompanying documentation for details on usage and
 * license.
 */

#ifndef _RADIUSSECRET_H__
#define _RADIUSSECRET_H__


#include "Radius.h"

#define D_SECRET_MAX_LENGTH           256


/**
 * @class RadiusSecret
 *
 * @brief RADIUS secret key
 *
 */
class RadiusSecret
{
public:
    /// Constructor
    RadiusSecret();

    /// Constructor
    /**
     * Construct secret key from NULL-terminated string (NULL 
     * terminator is not copied into the key). 
     * If you need to create secret key with non-readable 
     * characters, use setSecretKey() method instead. 
     *  
     * @param[int] p_string - NULL-terminated string
     */
    RadiusSecret(const char * p_string);

    /// Copy constructor
    RadiusSecret(const RadiusSecret & other);

    
    /// Get secret key
    /**
     * Get secret key.
     * Note that secret key may contain not-readable characters and should not be null-terminated.
     * Hence application should not use strcpy()-like functions on the returned value.
     * 
     * @param[out] p_data - pointer to the secret data
     * @param[out] p_length - length of secret key
     * @return RC_SUCCESS or RC_FAIL
     */
    int getSecret(const unsigned char * & p_data, uint16_t & p_length);
    
    /// Set secret key
    /**
     * Set secret key.
     * Note that secret key may contain not-readable characters and should not be null-terminated.
     * 
     * @param[in] p_data - pointer to the secret data
     * @param[in] p_length - length of secret key
     * @return RC_SUCCESS or RC_FAIL
     */
    int setSecret(const unsigned char * p_data, uint16_t p_length);


private:
    /// Secret key data
    unsigned char m_secret[D_SECRET_MAX_LENGTH];

    /// Secret key length
    uint16_t m_secretLength;

};


#endif // _RADIUSSECRET_H__


/*
 * This source file is part of the ace-radius library.  This code was
 * written by Alex Agranov in 2004-2009, and is covered by the BSD open source
 * license. Refer to the accompanying documentation for details on usage and
 * license.
 */

#include <stdio.h>
#include "afx.h"
#include <string.h>

#include "..\ace-radius/RadiusClientStack.h"

/*
 * A sample RADIUS client. 
 *  
 * Usage: 
 *     client [server-ip]
 *  
 * Notes: 
 *    - if server-ip is not specified, "localhost" is user instead
 *    - client sends Access-Request packet to authentication port (1812)
 *      with user="nemo" and password="arctangent"
 *    - response packet is dumped (if received)
 */

int Client (char *server, char *user, char *password, char *secret)
{
	int retval = 0;
    TRACE("Test RADIUS Client application\n");
    
#ifdef _MSC_VER
    WSADATA l_wsaData;
    if(WSAStartup(MAKEWORD(2,0),&l_wsaData) != 0)
    {
        TRACE("Can not initialize WinSock 2.0\n");
        return 1;
    }
#endif

	// create instance of client stack
    RadiusClientStack l_stack;
    if (!l_stack.isValid())
    {
        TRACE("Can not create RADIUS client stack\n");
#ifdef _MSC_VER
        WSACleanup();
#endif
        return 1;
    }

    // add connection with some RADIUS server
    l_stack.addServer(server, D_RADIUS_AUTHENTICATION_PORT, RadiusSecret(secret));
    
    // create a new packet
    RadiusPacket l_packet(D_PACKET_ACCESS_REQUEST, l_stack);
    
    // add some attributes to the packet
    RadiusAttribute l_attr;
    
    l_packet.addAttribute(D_ATTR_USER_NAME, l_attr);
    l_attr.setString(user);

    l_packet.addAttribute(D_ATTR_USER_PASSWORD, l_attr);
    l_attr.setUserPassword(password);

    struct in_addr l_addr;
#ifdef _MSC_VER
    l_addr.S_un.S_addr = inet_addr("192.168.1.4");
#else
    inet_aton("192.168.1.4", &l_addr);
#endif
    l_packet.addAttribute(D_ATTR_NAS_IP_ADDRESS, l_attr);
    l_attr.setIPAddress(l_addr);

    l_packet.addAttribute(D_ATTR_NAS_PORT, l_attr);
    l_attr.setNumber(3);

    l_packet.dump();

    // send the packet
    RadiusPacket * l_response = l_stack.sendPacket(l_packet);

    // print response if it was received
    if (l_response)
    {
        TRACE("---\nPacket was successfully transmitted\n---\n"); 
        TRACE("\nResponse packet:\n");
        l_response->dump();
		retval = 0;
    }
    else
	{
        TRACE("---\nPacket transmit failure\n---\n"); 
		retval = 1;
	}

#ifdef _MSC_VER
        WSACleanup();
#endif
    return retval;
}


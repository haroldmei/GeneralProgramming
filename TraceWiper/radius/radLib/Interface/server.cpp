/*
 * This source file is part of the ace-radius library.  This code was
 * written by Alex Agranov in 2004-2009, and is covered by the BSD open source
 * license. Refer to the accompanying documentation for details on usage and
 * license.
 */
#include "afx.h"
#include <stdio.h>
#include <string.h>

#include "..\..\..\istserver\src\cfgfile.h"

#include "..\ace-radius/RadiusServerStack.h"

/*
 * A sample RADIUS server. 
 *  
 * Usage: 
 *     server
 *  
 * Notes: 
 *   - server listens on authentication port (1812)
 *   - it answers only Access-Requests with user="nemo" and password="arctangent"
 */

// ----------------------
// verify request packet
// ----------------------
int verifyRequest(RadiusPacket & p_request, LegalItem *item)
{

    // everything looks OK
    return RC_SUCCESS;
}


int sendResponse(RadiusServerStack &p_stack)
{
    // ----------------------
    // build response packet
    // ----------------------
    RadiusPacket l_response(D_PACKET_ACCESS_ACCEPT, p_stack.getRequest());
    RadiusAttribute l_attr;

    // add Service-Type attribute
    l_response.addAttribute(D_ATTR_SERVICE_TYPE, l_attr);
    l_attr.setNumber(1);

    // add Login-Service attribute
    l_response.addAttribute(D_ATTR_LOGIN_SERVICE, l_attr);
    l_attr.setNumber(0);

    // add Login-IP-Host attribute
    l_response.addAttribute(D_ATTR_LOGIN_IP_HOST, l_attr);
    struct in_addr l_addr;
#ifdef _MSC_VER
    l_addr.S_un.S_addr = inet_addr("192.168.1.4");
#else
    inet_aton("192.168.1.4", &l_addr);
#endif
    l_attr.setIPAddress(l_addr);

    // ----------------------
    // send response packet
    // ----------------------
    p_stack.sendResponse(l_response);

    // it's important to dump response packet _after_ calling sendResponse() method,
    // since the latter modifies response autheticator
    TRACE("---\nSend response packet\n---\n");
    l_response.dump();

    return RC_SUCCESS;
}

DWORD WINAPI  Server(void*  lParam)// (int argc, char *argv[])
{
    int l_rc;
	LegalItem *item = NULL;

    TRACE("Test RADIUS Server application\n");

#ifdef _MSC_VER
    WSADATA l_wsaData;
    if(WSAStartup(MAKEWORD(2,0),&l_wsaData) != 0)
    {
        TRACE("Can not initialize WinSock 2.0\n");
        return 1;
    }
#endif

	// create instance of server stack
    RadiusServerStack l_stack(RadiusSecret("xyzzy5461"));
    if (!l_stack.isValid())
    {
        TRACE("Can not create RADIUS server stack\n");
#ifdef _MSC_VER
        WSACleanup();
#endif
        return 1;
    }
    
    // reference to request packet
    RadiusPacket & l_request = l_stack.getRequest();

    while (1)
    {
        l_rc = l_stack.receiveRequest();
        if (l_rc == RC_SUCCESS)
        {
            TRACE("---\nReceived request packet from ");
            l_stack.getClientAddress().dump();
            TRACE("\n---\n");
            l_request.dump();
			
			for (int i = 0; i < LegalItems.size(); i++)
			{
				TRACE("The index: %d\n", i);
				LegalItem *item = LegalItems.at(i);
				if (verifyRequest(l_stack.getRequest(), item) == RC_SUCCESS)
				{
					sendResponse(l_stack);
					break;
				}
			}
        }
        else
            TRACE("Error 1\n");
    }

#ifdef _MSC_VER
        WSACleanup();
#endif
    return 0;
}

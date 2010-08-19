/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2010                                                    *
 * Copyright (C) 1998-2010 WrightEagle 2D Soccer Simulation Team,                   *
 *                         Multi-Agent Systems Lab.,                                *
 *                         School of Computer Science and Technology,               *
 *                         University of Science and Technology of China, China.    *
 *                                                                                  *
 * This program is free software; you can redistribute it and/or                    *
 * modify it under the terms of the GNU General Public License                      *
 * as published by the Free Software Foundation; either version 2                   *
 * of the License, or (at your option) any later version.                           *
 *                                                                                  *
 * This program is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 * GNU General Public License for more details.                                     *
 *                                                                                  *
 * You should have received a copy of the GNU General Public License                *
 * along with this program; if not, write to the Free Software                      *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.  *
 ************************************************************************************/

#include "UDPSocket.h"

//==============================================================================
UDPSocket::UDPSocket()
{
	mIsInitialOK = false;
}


//==============================================================================
UDPSocket::~UDPSocket()
{
}


//==============================================================================
UDPSocket & UDPSocket::instance()
{
    static UDPSocket udp_socket;
    return udp_socket;
}


//==============================================================================
void UDPSocket::Initial(const char *host, int port)
{
	struct hostent *host_ent ;
	struct in_addr *addr_ptr ;

#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD( 2, 2 );
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		PRINT_ERROR("WSAStartup failed");
	}
	if (LOBYTE( wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		PRINT_ERROR("WSASTartup version mismatch");
	}
#endif

	if ((host_ent = (struct hostent *)gethostbyname(host)) == 0)
	{
		if (inet_addr(host) == INADDR_NONE)
			PRINT_ERROR("Invalid host name");
	}
	else
	{
		addr_ptr = (struct in_addr *) *host_ent->h_addr_list ;
		host = inet_ntoa(*addr_ptr) ;
	}

	if( (mSockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		PRINT_ERROR("Can't create socket") ;
	}

	memset(&mAddress, 0, sizeof(mAddress)) ;
	mAddress.sin_family         = AF_INET ;
	mAddress.sin_addr.s_addr    = htonl(INADDR_ANY) ;
	mAddress.sin_port           = htons(0) ;

	if(bind(mSockfd, (sockaddr *)&mAddress, sizeof(mAddress)) < 0)
	{
#ifdef WIN32
		closesocket(mSockfd);
#else
		close(mSockfd);
#endif
		PRINT_ERROR("Can't bind client to any port") ;
	}

	memset(&mAddress, 0, sizeof(mAddress)) ;
	mAddress.sin_family         = AF_INET ;
	mAddress.sin_addr.s_addr    = inet_addr( host ) ;
	mAddress.sin_port           = htons( port ) ;
	mIsInitialOK                = true;
}


//==============================================================================
int UDPSocket::Receive(char *msg)
{
#ifdef WIN32
	int servlen;
#else
	socklen_t servlen ;
#endif

	sockaddr_in serv_addr;
	servlen = sizeof(serv_addr);
	int n = recvfrom(mSockfd, msg, MAX_MESSAGE, 0, (sockaddr *)&serv_addr, &servlen);
	if (n > 0)
	{
		msg[n] = '\0' ; // rccparser will crash if msg has no end
		mAddress.sin_port = serv_addr.sin_port ;
	}

	return n ;
}


//==============================================================================
int UDPSocket::Send(const char *msg)
{
	if (mIsInitialOK == true)
	{
		int n = std::strlen(msg) ;
		n = sendto(mSockfd, msg, n+1, 0, (sockaddr *)&mAddress, sizeof(mAddress));
		return n;
	}

	std::cout << msg << std::endl;
	return std::strlen(msg);
}


//end of UDPSocket.cpp


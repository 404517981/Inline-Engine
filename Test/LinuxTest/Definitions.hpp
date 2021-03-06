#pragma once

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <WS2tcpip.h>
#undef SendMessage
#undef SetPort

typedef int socklen_t;

#else
	#include <stdio.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <sys/ioctl.h>
	#include <string.h>

	#define SOCKET_ERROR -1
	#define NO_ERROR 0
	#define INVALID_SOCKET NO_ERROR 

	#define SOCKET int
	#define closesocket close
	#define ioctlsocket ioctl

	inline int WSAGetLastError()
	{
		return 1;
	}

#endif

#define DEFAULT_LISTEN_PORT 61250

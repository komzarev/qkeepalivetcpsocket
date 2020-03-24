#include "keepalivetcpsocket.hpp"

#ifdef Q_OS_WIN
 #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
 #endif
 #pragma comment(lib, "Ws2_32.lib")
 #include <WinSock2.h>
 #include <mstcpip.h>
 #include <ws2ipdef.h>
#else
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netinet/tcp.h>
#endif

int keepalivetcpsocket::KeepAlive::setOptions(qintptr descriptor, bool isOn, int keepidle_sec , int keepcnt , int keepint_sec )
{
	int enableKeepAlive = isOn ? 1 : 0;

	if (descriptor == -1 || descriptor == 0) {
		return -1;
	}

#ifdef Q_OS_WIN
 #if WINVER >= _WIN32_WINNT_WIN10

	if (setsockopt(static_cast<SOCKET>(descriptor), SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char*>(&enableKeepAlive), sizeof(enableKeepAlive)) != 0) {
		return WSAGetLastError();
    }

    if (isOn){

		if (setsockopt(static_cast<SOCKET>(descriptor), IPPROTO_TCP, TCP_KEEPIDLE, reinterpret_cast<char*>(&keepidle_sec), sizeof(keepidle_sec)) != 0
			|| setsockopt(static_cast<SOCKET>(descriptor), IPPROTO_TCP, TCP_KEEPCNT, reinterpret_cast<char*>(&keepcnt), sizeof(keepcnt)) != 0
			|| setsockopt(static_cast<SOCKET>(descriptor), IPPROTO_TCP, TCP_KEEPINTVL, reinterpret_cast<char*>(&keepint_sec), sizeof(keepint_sec)) != 0)
		{
			return WSAGetLastError();
		}
    }

 #else

#ifdef _MSC_VER
	#pragma message("WARNING: Keepalive in not fully supported on this version of windows")
#else
	#warning(Keepalive in not fully supported on this version of windows)
#endif

	DWORD dwBytes;
	tcp_keepalive pClSock_tcpKeepalive;
	pClSock_tcpKeepalive.onoff = enableKeepAlive;
	pClSock_tcpKeepalive.keepalivetime = keepidle_sec * 1000;
	pClSock_tcpKeepalive.keepaliveinterval = keepint_sec * 1000;
	if (WSAIoctl(descriptor, SIO_KEEPALIVE_VALS, &pClSock_tcpKeepalive, sizeof(pClSock_tcpKeepalive), NULL, 0, &dwBytes, NULL, NULL) != 0) {
		return WSAGetLastError();
	}

 #endif

#else

	if (setsockopt(static_cast<SOCKET>(descriptor), SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char*>(&enableKeepAlive), sizeof(enableKeepAlive)) != 0) {
        return errno;
	}

	if (isOn) {

		if (setsockopt(descriptor, SOL_TCP, TCP_KEEPIDLE, &keepidle_sec, sizeof(keepidle_sec)) != 0
			|| setsockopt(descriptor, SOL_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt)) != 0
			|| setsockopt(descriptor, SOL_TCP, TCP_KEEPINTVL, &keepint_sec, sizeof(keepint_sec)) != 0)
		{
            return errno;
		}
    }

#endif

	return 0;
}

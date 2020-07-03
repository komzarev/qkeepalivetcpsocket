#include "keepalivetcpoption.hpp"

#include <cstring>

#ifdef _WIN32

#   ifdef _MSC_VER
#   pragma comment(lib, "Ws2_32.lib")
#   endif

#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif

#   include <winsock2.h>
#   include <mstcpip.h>
#   include <ws2ipdef.h>

#   ifndef TCP_KEEPALIVE //mingw miss this values in ws2ipdef.h
#       define TCP_KEEPALIVE            3
#       define TCP_KEEPCNT              16
#       define TCP_KEEPIDLE             TCP_KEEPALIVE
#       define TCP_KEEPINTVL            17
#   endif

#else
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <netinet/tcp.h>
#endif

#ifdef __QNXNTO__
#   include <errno.h>
#   include <sys/types.h>
#   include <sys/param.h>
#   include <sys/sysctl.h>
#   include <netinet/tcp_var.h>
#endif

int keepalivetcpoption::setKeepAliveOptions(std::intptr_t descriptor, bool isOn, std::chrono::seconds idleBeforeProbe , unsigned int probesCount , std::chrono::seconds responseTimeout )
{
    int keepidle_sec = idleBeforeProbe.count();
    int keepcnt = probesCount;
    int keepint_sec = responseTimeout.count();

    int enableKeepAlive = isOn ? 1 : 0;

    if (descriptor == -1 || descriptor == 0) {
        return -1;
    }

#ifdef _WIN32
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
    if (setsockopt(static_cast<int>(descriptor), SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<void *>(&enableKeepAlive), sizeof(enableKeepAlive)) != 0) {
        return errno;
    }

    if (isOn) {
#ifdef __QNXNTO__
        int mib[4];
        int ival;
        struct timeval tval;

        mib[0] = CTL_NET;
        mib[1] = AF_INET;
        mib[2] = IPPROTO_TCP;
        mib[3] = TCPCTL_KEEPCNT;
        ival = keepcnt; /* Number of keepalive probe attempts  (default is 8) */

        if(sysctl(mib, 4, NULL, NULL, &ival, sizeof(ival)))
        {
            return errno;
        }

        mib[0] = CTL_NET;
        mib[1] = AF_INET;
        mib[2] = IPPROTO_TCP;
        mib[3] = TCPCTL_KEEPINTVL;
        ival = keepint_sec * 2; /* Half seconds between probe attempts; default is 150 (75 sec) */

        if(sysctl(mib, 4, NULL, NULL, &ival, sizeof(ival)))
        {
            return errno;
        }

        ::memset(&tval, 0, sizeof(tval));
        tval.tv_sec = keepidle_sec;  /* Seconds of idle time before probing
                              starts (default is 7200) */

        if(setsockopt(static_cast<int>(descriptor), IPPROTO_TCP, TCP_KEEPALIVE, (void *) &tval,
                          sizeof(tval)))
        {
            return errno;
        }
#else //*nix
        const int user_timeout_msec = keepidle_sec * 1000 + keepint_sec * 1000 * keepcnt - keepint_sec * 500;
        if (setsockopt(static_cast<int>(descriptor), SOL_TCP, TCP_KEEPIDLE, &keepidle_sec, sizeof(keepidle_sec)) != 0
            || setsockopt(static_cast<int>(descriptor), SOL_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt)) != 0
            || setsockopt(static_cast<int>(descriptor), SOL_TCP, TCP_KEEPINTVL, &keepint_sec, sizeof(keepint_sec)) != 0
            || setsockopt(static_cast<int>(descriptor), SOL_TCP, TCP_USER_TIMEOUT, &user_timeout_msec, sizeof(user_timeout_msec)) != 0)
        {
            return errno;
        }
#endif
    }
#endif

    return 0;
}

/*
 *  TCP/IP or UDP/IP networking functions
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_NET_C)

#if !defined(unix) && !defined(__unix__) && !defined(__unix) && \
    !defined(__APPLE__) && !defined(_WIN32)
#error "This module only works on Unix and Windows, see MBEDTLS_NET_C in config.h"
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/mbedtls_platform.h"
#else
#include <stdlib.h>
#endif

#include "mbedtls/net_sockets.h"

#include <string.h>



#if (defined(_WIN32) || defined(_WIN32_WCE)) && !defined(EFIX64) && \
    !defined(EFI32)

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
/* Enables getaddrinfo() & Co */
#define _WIN32_WINNT 0x0501
#include <ws2tcpip.h>

#include <winsock2.h>
#include <windows.h>

#if defined(_MSC_VER)
#if defined(_WIN32_WCE)
#pragma comment( lib, "ws2.lib" )
#else
#pragma comment( lib, "ws2_32.lib" )
#endif
#endif /* _MSC_VER */

#define read(fd,buf,len)        recv( fd, (char*)( buf ), (int)( len ), 0 )
#define write(fd,buf,len)       send( fd, (char*)( buf ), (int)( len ), 0 )
#define close(fd)               closesocket(fd)

static int wsa_init_done = 0;

#else /* ( _WIN32 || _WIN32_WCE ) && !EFIX64 && !EFI32 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>



#endif /* ( _WIN32 || _WIN32_WCE ) && !EFIX64 && !EFI32 */

/* Some MS functions want int and MSVC warns if we pass size_t,
 * but the standard functions use socklen_t, so cast only for MSVC */
#if defined(_MSC_VER)
#define MSVC_INT_CAST   (int)
#else
#define MSVC_INT_CAST
#endif

#include <stdio.h>

#include <time.h>

#include <stdint.h>

/*
 * Prepare for using the sockets interface
 */
static int net_prepare( void )
{
#if ( defined(_WIN32) || defined(_WIN32_WCE) ) && !defined(EFIX64) && \
    !defined(EFI32)
    WSADATA wsaData;

    if( wsa_init_done == 0 )
    {
        if( WSAStartup( MAKEWORD(2,0), &wsaData ) != 0 )
            return( MBEDTLS_ERR_NET_SOCKET_FAILED );

        wsa_init_done = 1;
    }
#else
#if !defined(EFIX64) && !defined(EFI32)
    signal( SIGPIPE, SIG_IGN );
#endif
#endif
    return( 0 );
}

/*
 * Initialize a context
 */
void mbedtls_net_init( mbedtls_net_context *ctx )
{
    ctx->fd = -1;
}

/*
 * Initiate a TCP connection with host:port and the given protocol
 */
int mbedtls_net_connect( mbedtls_net_context *ctx, const char *host,
                         const char *port, int proto )
{
    int ret;
    struct addrinfo hints, *addr_list, *cur;

    if( ( ret = net_prepare() ) != 0 )
        return( ret );

    /* Do name resolution with both IPv6 and IPv4 */
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = proto == MBEDTLS_NET_PROTO_UDP ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_protocol = proto == MBEDTLS_NET_PROTO_UDP ? IPPROTO_UDP : IPPROTO_TCP;

    if( getaddrinfo( host, port, &hints, &addr_list ) != 0 )
        return( MBEDTLS_ERR_NET_UNKNOWN_HOST );

    /* Try the sockaddrs until a connection succeeds */
    ret = MBEDTLS_ERR_NET_UNKNOWN_HOST;
    for( cur = addr_list; cur != NULL; cur = cur->ai_next )
    {
        ctx->fd = (int) socket( cur->ai_family, cur->ai_socktype,
                            cur->ai_protocol );
        if( ctx->fd < 0 )
        {
            ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
            continue;
        }

        if( connect( ctx->fd, cur->ai_addr, MSVC_INT_CAST cur->ai_addrlen ) == 0 )
        {
            ret = 0;
            break;
        }

        close( ctx->fd );
        ret = MBEDTLS_ERR_NET_CONNECT_FAILED;
    }

    freeaddrinfo( addr_list );

    return( ret );
}

/*
 * Create a listening socket on bind_ip:port
 */
int mbedtls_net_bind( mbedtls_net_context *ctx, const char *bind_ip, const char *port, int proto )
{
    int n, ret;
    struct addrinfo hints, *addr_list, *cur;

    if( ( ret = net_prepare() ) != 0 )
        return( ret );

    /* Bind to IPv6 and/or IPv4, but only in the desired protocol */
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = proto == MBEDTLS_NET_PROTO_UDP ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_protocol = proto == MBEDTLS_NET_PROTO_UDP ? IPPROTO_UDP : IPPROTO_TCP;
    if( bind_ip == NULL )
        hints.ai_flags = AI_PASSIVE;

    if( getaddrinfo( bind_ip, port, &hints, &addr_list ) != 0 )
        return( MBEDTLS_ERR_NET_UNKNOWN_HOST );

    /* Try the sockaddrs until a binding succeeds */
    ret = MBEDTLS_ERR_NET_UNKNOWN_HOST;
    for( cur = addr_list; cur != NULL; cur = cur->ai_next )
    {
        ctx->fd = (int) socket( cur->ai_family, cur->ai_socktype,
                            cur->ai_protocol );
        if( ctx->fd < 0 )
        {
            ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
            continue;
        }

        n = 1;
        if( setsockopt( ctx->fd, SOL_SOCKET, SO_REUSEADDR,
                        (const char *) &n, sizeof( n ) ) != 0 )
        {
            close( ctx->fd );
            ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
            continue;
        }

        if( bind( ctx->fd, cur->ai_addr, MSVC_INT_CAST cur->ai_addrlen ) != 0 )
        {
            close( ctx->fd );
            ret = MBEDTLS_ERR_NET_BIND_FAILED;
            continue;
        }

        /* Listen only makes sense for TCP */
        if( proto == MBEDTLS_NET_PROTO_TCP )
        {
            if( listen( ctx->fd, MBEDTLS_NET_LISTEN_BACKLOG ) != 0 )
            {
                close( ctx->fd );
                ret = MBEDTLS_ERR_NET_LISTEN_FAILED;
                continue;
            }
        }

        /* Bind was successful */
        ret = 0;
        break;
    }

    freeaddrinfo( addr_list );

    return( ret );

}

#if ( defined(_WIN32) || defined(_WIN32_WCE) ) && !defined(EFIX64) && \
    !defined(EFI32)
/*
 * Check if the requested operation would be blocking on a non-blocking socket
 * and thus 'failed' with a negative return value.
 */
static int net_would_block( const mbedtls_net_context *ctx )
{
    ((void) ctx);
    return( WSAGetLastError() == WSAEWOULDBLOCK );
}
#else
/*
 * Check if the requested operation would be blocking on a non-blocking socket
 * and thus 'failed' with a negative return value.
 *
 * Note: on a blocking socket this function always returns 0!
 */
static int net_would_block( const mbedtls_net_context *ctx )
{
    int err = errno;
    
    /*
     * Never return 'WOULD BLOCK' on a non-blocking socket
     */
    if( ( fcntl( ctx->fd, F_GETFL ) & O_NONBLOCK ) != O_NONBLOCK )
    {
        errno = err;
        return( 0 );
    }

    switch( errno = err )
    {
#if defined EAGAIN
        case EAGAIN:
#endif
#if defined EWOULDBLOCK && EWOULDBLOCK != EAGAIN
        case EWOULDBLOCK:
#endif
            return( 1 );
    }
    return( 0 );
}
#endif /* ( _WIN32 || _WIN32_WCE ) && !EFIX64 && !EFI32 */

/*
 * Accept a connection from a remote client
 */
int mbedtls_net_accept( mbedtls_net_context *bind_ctx,
                        mbedtls_net_context *client_ctx,
                        void *client_ip, size_t buf_size, size_t *ip_len )
{
    int ret;
    int type;

    struct sockaddr_storage client_addr;

#if defined(__socklen_t_defined) || defined(_SOCKLEN_T) ||  \
    defined(_SOCKLEN_T_DECLARED) || defined(__DEFINED_socklen_t)
    socklen_t n = (socklen_t) sizeof( client_addr );
    socklen_t type_len = (socklen_t) sizeof( type );
#else
    int n = (int) sizeof( client_addr );
    int type_len = (int) sizeof( type );
#endif

    /* Is this a TCP or UDP socket? */
    if( getsockopt( bind_ctx->fd, SOL_SOCKET, SO_TYPE,
                    (void *) &type, &type_len ) != 0 ||
        ( type != SOCK_STREAM && type != SOCK_DGRAM ) )
    {
        return( MBEDTLS_ERR_NET_ACCEPT_FAILED );
    }

    if( type == SOCK_STREAM )
    {
        /* TCP: actual accept() */
        ret = client_ctx->fd = (int) accept( bind_ctx->fd,
                                             (struct sockaddr *) &client_addr, &n );
    }
    else
    {
        /* UDP: wait for a message, but keep it in the queue */
        char buf[1] = { 0 };

        ret = (int) recvfrom( bind_ctx->fd, buf, sizeof( buf ), MSG_PEEK,
                        (struct sockaddr *) &client_addr, &n );

#if defined(_WIN32)
        if( ret == SOCKET_ERROR &&
            WSAGetLastError() == WSAEMSGSIZE )
        {
            /* We know buf is too small, thanks, just peeking here */
            ret = 0;
        }
#endif
    }

    if( ret < 0 )
    {
        if( net_would_block( bind_ctx ) != 0 )
            return( MBEDTLS_ERR_SSL_WANT_READ );

        return( MBEDTLS_ERR_NET_ACCEPT_FAILED );
    }

    /* UDP: hijack the listening socket to communicate with the client,
     * then bind a new socket to accept new connections */
    if( type != SOCK_STREAM )
    {
        struct sockaddr_storage local_addr;
        int one = 1;

        if( connect( bind_ctx->fd, (struct sockaddr *) &client_addr, n ) != 0 )
            return( MBEDTLS_ERR_NET_ACCEPT_FAILED );

        client_ctx->fd = bind_ctx->fd;
        bind_ctx->fd   = -1; /* In case we exit early */

        n = sizeof( struct sockaddr_storage );
        if( getsockname( client_ctx->fd,
                         (struct sockaddr *) &local_addr, &n ) != 0 ||
            ( bind_ctx->fd = (int) socket( local_addr.ss_family,
                                           SOCK_DGRAM, IPPROTO_UDP ) ) < 0 ||
            setsockopt( bind_ctx->fd, SOL_SOCKET, SO_REUSEADDR,
                        (const char *) &one, sizeof( one ) ) != 0 )
        {
            return( MBEDTLS_ERR_NET_SOCKET_FAILED );
        }

        if( bind( bind_ctx->fd, (struct sockaddr *) &local_addr, n ) != 0 )
        {
            return( MBEDTLS_ERR_NET_BIND_FAILED );
        }
    }

    if( client_ip != NULL )
    {
        if( client_addr.ss_family == AF_INET )
        {
            struct sockaddr_in *addr4 = (struct sockaddr_in *) &client_addr;
            *ip_len = sizeof( addr4->sin_addr.s_addr );

            if( buf_size < *ip_len )
                return( MBEDTLS_ERR_NET_BUFFER_TOO_SMALL );

            memcpy( client_ip, &addr4->sin_addr.s_addr, *ip_len );
        }
        else
        {
            struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) &client_addr;
            *ip_len = sizeof( addr6->sin6_addr.s6_addr );

            if( buf_size < *ip_len )
                return( MBEDTLS_ERR_NET_BUFFER_TOO_SMALL );

            memcpy( client_ip, &addr6->sin6_addr.s6_addr, *ip_len);
        }
    }

    return( 0 );
}

/*
 * Set the socket blocking or non-blocking
 */
int mbedtls_net_set_block( mbedtls_net_context *ctx )
{
    u_long n = 0;
    return( ioctlsocket( ctx->fd, FIONBIO, &n ) );
/*
#if ( defined(_WIN32) || defined(_WIN32_WCE) ) && !defined(EFIX64) && \
    !defined(EFI32)
#else
    return( fcntl( ctx->fd, F_SETFL, fcntl( ctx->fd, F_GETFL ) & ~O_NONBLOCK ) );
#endif*/
}

int mbedtls_net_set_nonblock( mbedtls_net_context *ctx )
{
    u_long n = 1;
    return( ioctlsocket( ctx->fd, FIONBIO, &n ) );
	/*
#if ( defined(_WIN32) || defined(_WIN32_WCE) ) && !defined(EFIX64) && \
    !defined(EFI32)
#else
    return( fcntl( ctx->fd, F_SETFL, fcntl( ctx->fd, F_GETFL ) | O_NONBLOCK ) );
#endif
*/
}

/*
 * Portable usleep helper
 */
void mbedtls_net_usleep( unsigned long usec )
{
#if defined(_WIN32)
    Sleep( ( usec + 999 ) / 1000 );
#else
    struct timeval tv;
    tv.tv_sec  = usec / 1000000;
#if defined(__unix__) || defined(__unix) || \
    ( defined(__APPLE__) && defined(__MACH__) )
    tv.tv_usec = (suseconds_t) usec % 1000000;
#else
    tv.tv_usec = usec % 1000000;
#endif
    select( 0, NULL, NULL, NULL, &tv );
#endif
}

/*
 * Read at most 'len' characters
 */
int mbedtls_net_recv( void *ctx, unsigned char *buf, size_t len )
{
    int ret;
    int fd = ((mbedtls_net_context *) ctx)->fd;

    if( fd < 0 )
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );

    ret = (int) read( fd, buf, len );

    if( ret < 0 )
    {
        if( net_would_block( ctx ) != 0 )
            return( MBEDTLS_ERR_SSL_WANT_READ );

#if ( defined(_WIN32) || defined(_WIN32_WCE) ) && !defined(EFIX64) && \
    !defined(EFI32)
        if( WSAGetLastError() == WSAECONNRESET )
            return( MBEDTLS_ERR_NET_CONN_RESET );
#else
        if( errno == EPIPE || errno == ECONNRESET )
            return( MBEDTLS_ERR_NET_CONN_RESET );

        if( errno == EINTR )
            return( MBEDTLS_ERR_SSL_WANT_READ );
#endif

        return( MBEDTLS_ERR_NET_RECV_FAILED );
    }

    return( ret );
}

/*
 * Read at most 'len' characters, blocking for at most 'timeout' ms
 */
int mbedtls_net_recv_timeout( void *ctx, unsigned char *buf, size_t len,
                      uint32_t timeout )
{
    int ret;
    struct timeval tv;
    fd_set read_fds;
    int fd = ((mbedtls_net_context *) ctx)->fd;

    if( fd < 0 )
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );

    FD_ZERO( &read_fds );
    FD_SET( fd, &read_fds );

    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = ( timeout % 1000 ) * 1000;

    ret = select( fd + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv );

    /* Zero fds ready means we timed out */
    if( ret == 0 )
        return( MBEDTLS_ERR_SSL_TIMEOUT );

    if( ret < 0 )
    {
#if ( defined(_WIN32) || defined(_WIN32_WCE) ) && !defined(EFIX64) && \
    !defined(EFI32)
        if( WSAGetLastError() == WSAEINTR )
            return( MBEDTLS_ERR_SSL_WANT_READ );
#else
        if( errno == EINTR )
            return( MBEDTLS_ERR_SSL_WANT_READ );
#endif

        return( MBEDTLS_ERR_NET_RECV_FAILED );
    }

    /* This call will not block */
    return( mbedtls_net_recv( ctx, buf, len ) );
}

/*
 * Write at most 'len' characters
 */
int mbedtls_net_send( void *ctx, const unsigned char *buf, size_t len )
{
    int ret;
    int fd = ((mbedtls_net_context *) ctx)->fd;

    if( fd < 0 )
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );

    ret = (int) write( fd, buf, len );

    if( ret < 0 )
    {
        if( net_would_block( ctx ) != 0 )
            return( MBEDTLS_ERR_SSL_WANT_WRITE );

#if ( defined(_WIN32) || defined(_WIN32_WCE) ) && !defined(EFIX64) && \
    !defined(EFI32)
        if( WSAGetLastError() == WSAECONNRESET )
            return( MBEDTLS_ERR_NET_CONN_RESET );
#else
        if( errno == EPIPE || errno == ECONNRESET )
            return( MBEDTLS_ERR_NET_CONN_RESET );

        if( errno == EINTR )
            return( MBEDTLS_ERR_SSL_WANT_WRITE );
#endif

        return( MBEDTLS_ERR_NET_SEND_FAILED );
    }

    return( ret );
}

/*
 * Gracefully close the connection
 */
void mbedtls_net_free( mbedtls_net_context *ctx )
{
    if( ctx->fd == -1 )
        return;

    shutdown( ctx->fd, 2 );
    close( ctx->fd );

    ctx->fd = -1;
}

#else /* MBEDTLS_NET_C */


#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/inet.h"
//#include <errno.h>
//#include <time.h>
typedef unsigned char wint_t;

#include <stdio.h>
//#define __time_t_defined
//#include <sys/types.h>
//#include <unistd.h>
//#include <fcntl.h>

#include <sockets.h>
//#include "lwip/netdb.h"
#include "mbedtls/net.h"


#ifndef F_GETFL
#define F_GETFL 3
#endif
#ifndef F_SETFL
#define F_SETFL 4
#endif


static int wsa_init_done = 0;

//#define mbedtls_sck_printf iot_printf
#define mbedtls_sck_printf // 

/*
 * Prepare for using the sockets interface
 */
FLASH_FUNC static int net_prepare( void )
{
    return( 0 );
}


/*
void uHAL_Printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}*/

/*
 * Initialize a context
 */
FLASH_FUNC void mbedtls_net_init( mbedtls_net_context *ctx )
{
    ctx->fd = -1;
}

/*
void test_print()
{
	uHAL_Printf("mbedtls_net_connect xxx\n");
		HAL_SleepMs(20);
		uHAL_Printf("mbedtls_net_connect xxx\n");

}*/

/*
 * Initiate a TCP connection with host:port and the given protocol
 */
 #define SDK_GET_HOST_BY_NAME_
FLASH_FUNC int mbedtls_net_connect( mbedtls_net_context *ctx, const char *host, const char *port, int proto )
{
	iot_printf("mbedtls_net_connect xxx\n");
    int ret;
    unsigned int ports = atoi(port);
  
    struct sockaddr_in server_addr;
	int protocol = 0;
	int sock_type = 0;

    if( ( ret = net_prepare() ) != 0 )
        return( ret );
	//uHAL_Printf("mbedtls_net_connect xxx\n");
	 //HAL_SleepMs(20);

    protocol = (proto == MBEDTLS_NET_PROTO_UDP)?(IPPROTO_UDP):(IPPROTO_TCP);
    sock_type = (proto == MBEDTLS_NET_PROTO_UDP)?(SOCK_DGRAM):(SOCK_STREAM);
    ctx->fd = socket(AF_INET, sock_type, protocol);
	if( ctx->fd < 0 ){
		ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
		mbedtls_sck_printf("socket() failed in mbedtls_net_connect!\n");
		goto mbedtls_error;
	}	

	server_addr.sin_family = AF_INET;
#ifdef SDK_GET_HOST_BY_NAME_
		struct hostent *hostinfo;
		iot_printf("gethostbyname = %s\n",host);
		if((hostinfo = gethostbyname(host)) == NULL)
		{
			iot_printf("gethostbyname error\n");
			return MBEDTLS_ERR_NET_SOCKET_FAILED;
		}
		iot_printf("gethostbyname ++++\n");
		server_addr.sin_addr = *((struct in_addr *)hostinfo->h_addr);
#else
	   server_addr.sin_addr.s_addr = inet_addr(host);
#endif	
	//server_addr.sin_addr.s_addr = inet_addr(host);
	server_addr.sin_port = htons(ports);

	ret = connect(ctx->fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	iot_printf("connect() OK in mbedtls_net_connect!ctx->fd: %d\n", ctx->fd);
	if(ret < 0){
		ret = MBEDTLS_ERR_NET_CONNECT_FAILED;
		mbedtls_sck_printf("connect() failed in mbedtls_net_connect!\n");
	}

mbedtls_error:
	if (ret < 0)
		close( ctx->fd );
    return( ret );
}

/*
 * Create a listening socket on bind_ip:port
 */
FLASH_FUNC int mbedtls_net_bind( mbedtls_net_context *ctx, const char *bind_ip, const char *port, int proto )
{
	int ret;
	int protocol = 0;
	int sock_type = 0;
	unsigned int ports = atoi(port);

	struct sockaddr_in server_addr;

    if( ( ret = net_prepare() ) != 0 )
        return( ret );

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(((u32_t)0x00000000UL));
    server_addr.sin_port = htons(ports);
    protocol = (proto == MBEDTLS_NET_PROTO_UDP)?(IPPROTO_UDP):(IPPROTO_TCP);
    sock_type = (proto == MBEDTLS_NET_PROTO_UDP)?(SOCK_DGRAM):(SOCK_STREAM);

    ctx->fd = socket(AF_INET, sock_type, protocol);
    if( ctx->fd < 0 ){
		ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
		mbedtls_sck_printf("socket() failed in mbedtls_net_bind!\n");
		goto mbedtls_error;
	}

    ret =  bind(ctx->fd, (struct sockaddr*)&server_addr,sizeof(server_addr));
    if (ret != 0){
       ret = MBEDTLS_ERR_NET_BIND_FAILED;
       mbedtls_sck_printf("bind() failed in mbedtls_net_bind!\n");
       goto mbedtls_error;
    }

    if (proto == MBEDTLS_NET_PROTO_TCP){
    	ret = listen(ctx->fd, MBEDTLS_ERR_NET_LISTEN_FAILED);
    	if (ret != 0){
    		ret = MBEDTLS_ERR_NET_LISTEN_FAILED;
    		goto mbedtls_error;
    	}
    }

    ret = 0;

    mbedtls_error:
    	if (ret < 0)
    		close( ctx->fd );
        return( ret );

}
/*
get_non_blocking

input
fd: socket fd

output
1: NonBlocking mode
0: blocking mode
*/
int get_non_blocking(const mbedtls_net_context *ctx )
{	
	struct lwip_socket *sock;
	sock = get_socket(ctx->fd);
	return  (((sock)->flags & O_NONBLOCK) != 0);
}

/*
 * Check if the requested operation would be blocking on a non-blocking socket
 * and thus 'failed' with a negative return value.
 *
 * Note: on a blocking socket this function always returns 0!
 */
FLASH_FUNC static int net_would_block( const mbedtls_net_context *ctx )
{
    /*
     * Never return 'WOULD BLOCK' on a non-blocking socket
     */
#if 1   
			if(get_non_blocking(ctx ) == 0)  
			{
			 // printf("net_would_block: block mode\n");
				return( 0 );
			}
			//printf("net_would_block:NON block mode\n");
			switch( errno )
			{
#if defined EAGAIN
				case EAGAIN:
#endif
#if defined EWOULDBLOCK && EWOULDBLOCK != EAGAIN
				case EWOULDBLOCK:
#endif
					return( 1 );
			}
#endif

    return( 0 );
}

/*
 * Accept a connection from a remote client
 */
FLASH_FUNC int mbedtls_net_accept( mbedtls_net_context *bind_ctx,
                        mbedtls_net_context *client_ctx,
                        void *client_ip, size_t buf_size, size_t *ip_len )
{
    int ret;
    int type;
    struct sockaddr_in client_addr;

    socklen_t n = (socklen_t) sizeof( client_addr );
    socklen_t type_len = (socklen_t) sizeof( type );

	client_ctx->fd = (int) accept( bind_ctx->fd,(struct sockaddr *) &client_addr, &n );
	if (client_ctx->fd < 0){
		ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
		mbedtls_sck_printf("Create Socket Failed!\n");
		goto mbedtls_error;
	}

	if(client_ip != NULL){
		struct sockaddr_in *addr4 = (struct sockaddr_in *) &client_addr;
		*ip_len = sizeof( addr4->sin_addr.s_addr );

		if( buf_size < *ip_len ){
			ret = MBEDTLS_ERR_NET_BUFFER_TOO_SMALL;
			mbedtls_sck_printf("Create Socket Failed!\n");
			goto mbedtls_error;
		}

		memcpy(client_ip, &addr4->sin_addr.s_addr, *ip_len);
	}
	ret = 0;
mbedtls_error:
	if (ret < 0)
		close( client_ctx->fd );
    return( ret );
    return( 0 );
}

/*
 * Set the socket blocking or non-blocking
 */
FLASH_FUNC int mbedtls_net_set_block( mbedtls_net_context *ctx )
{
	u32 n = 0;
    return( ioctlsocket( ctx->fd, FIONBIO, &n ) );
/*
#if ( defined(_WIN32) || defined(_WIN32_WCE) ) && !defined(EFIX64) && \
    !defined(EFI32)
    u_long n = 0;
    return( ioctlsocket( ctx->fd, FIONBIO, &n ) );
#else
    return( fcntl( ctx->fd, F_SETFL, fcntl( ctx->fd, F_GETFL, 0 ) & ~O_NONBLOCK ) );
#endif
*/
}

FLASH_FUNC int mbedtls_net_set_nonblock( mbedtls_net_context *ctx )
{
    u32 n = 1;
    return( ioctlsocket( ctx->fd, FIONBIO, &n ) );
	/*
#if ( defined(_WIN32) || defined(_WIN32_WCE) ) && !defined(EFIX64) && \
    !defined(EFI32)
    u_long n = 1;
    return( ioctlsocket( ctx->fd, FIONBIO, &n ) );
#else
    return( fcntl( ctx->fd, F_SETFL, fcntl( ctx->fd, F_GETFL, 0 ) | O_NONBLOCK ) );
#endif
*/
}

/*
 * Portable usleep helper
 */
FLASH_FUNC void mbedtls_net_usleep( unsigned long usec )
{
//#if defined(_WIN32)
//    Sleep( ( usec + 999 ) / 1000 );
//#else
//    struct timeval tv;
//    tv.tv_sec  = usec / 1000000;
//#if defined(__unix__) || defined(__unix) || \
//    ( defined(__APPLE__) && defined(__MACH__) )
//    tv.tv_usec = (suseconds_t) usec % 1000000;
//#else
//tv.tv_usec = usec % 1000000;
//#endif
//  select( 0, NULL, NULL, NULL, &tv );
//#endif
}

/*
 * Read at most 'len' characters
 */
 extern uint32 HAL_TIME_MS();
FLASH_FUNC int mbedtls_net_recv( void *ctx, unsigned char *buf, size_t len )
{
    int ret;
	
    int fd = ((mbedtls_net_context *) ctx)->fd;

    if( fd < 0 )
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );
	

    ret = (int) read( fd, buf, len );

    if( ret < 0 )
    {
        if( net_would_block( ctx ) != 0 )
            return( MBEDTLS_ERR_SSL_WANT_READ );

        return( MBEDTLS_ERR_NET_RECV_FAILED );
    }
	
    //if (ret == 0)
    //	return( MBEDTLS_ERR_SSL_CONN_EOF );

    return( ret );
}

/*
 * Read at most 'len' characters, blocking for at most 'timeout' ms
 */
FLASH_FUNC int mbedtls_net_recv_timeout( void *ctx, unsigned char *buf, size_t len,
                      uint32_t timeout )
{
    int ret;
    struct timeval tv;
    fd_set read_fds;
    int fd = ((mbedtls_net_context *) ctx)->fd;

    if( fd < 0 )
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );

    FD_ZERO( &read_fds );
    FD_SET( fd, &read_fds );

    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = ( timeout % 1000 ) * 1000;

    ret = select( fd + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv );

    /* Zero fds ready means we timed out */
    if( ret == 0 )
        return( MBEDTLS_ERR_SSL_TIMEOUT );

    if( ret < 0 )
    {
        if( errno == EINTR )
            return( MBEDTLS_ERR_SSL_WANT_READ );

        return( MBEDTLS_ERR_NET_RECV_FAILED );
    }

    /* This call will not block */
    return( mbedtls_net_recv( ctx, buf, len ) );
}


/*
 * Write at most 'len' characters
 */
FLASH_FUNC int mbedtls_net_send( void *ctx, const unsigned char *buf, size_t len )
{
    int ret;
    int fd = ((mbedtls_net_context *) ctx)->fd;

    if( fd < 0 )
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );

    ret = (int) write( fd, buf, len );

    if( ret < 0 )
    {
        if( net_would_block( ctx ) != 0 )
            return( MBEDTLS_ERR_SSL_WANT_WRITE );

    }
    //if (ret == 0)
    //    return( MBEDTLS_ERR_SSL_WANT_WRITE );
    return( ret );
}

/*
 * Gracefully close the connection
 */
FLASH_FUNC void mbedtls_net_free( mbedtls_net_context *ctx )
{
    if( ctx->fd == -1 )
        return;

    shutdown( ctx->fd, 2 );
    close( ctx->fd );

    ctx->fd = -1;
}


#endif /* MBEDTLS_NET_C */

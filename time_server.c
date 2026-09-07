/*itme_server.c*/

#if defined(_WIN32)
#ifndef _WIN32_WINNINT 
#define _WIN32_WINNINT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#endif

#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())

#else
#define ISVALIDSOCKET(s) ((s)>=0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>

int main()
{
#if defined(_WIN32)
	WSADATA d;
	if(WSAStartup(MAKEWORD(2,2), &d)){
		fprintf(stderr,"Failed to initialize.\n");
		return 1;
	}

#endif
	printf("Configuring local address...\n");
	
	struct addrinfo hints;




	//
	//
	//The addrinfo structure used by getaddrinfo() contains the
      	// following fields:

        //   struct addrinfo {
        //       int              ai_flags;
        //       int              ai_family;
        //       int              ai_socktype;
        //       int              ai_protocol;
        //       socklen_t        ai_addrlen;
        //       struct sockaddr *ai_addr;
        //       char            *ai_canonname;
        //       struct addrinfo *ai_next;
        //   };


	memset(&hints,0,sizeof(hints));
	//initialize the hints data structre to 0


	hints.ai_family = AF_INET;
	//This field specifies the desired address family for the
        //     returned addresses.  Valid values for this field include
        //     AF_INET and AF_INET6.  The value AF_UNSPEC indicates that
        //     getaddrinfo() should return socket addresses for any
        //     address family (either IPv4 or IPv6, for example) that can
        //     be used with node and service.
	hints.ai_socktype = SOCK_STREAM;
	//
	//This field specifies the preferred socket type, for example
        //     SOCK_STREAM or SOCK_DGRAM.  Specifying 0 in this field
        //     indicates that socket addresses of any type can be returned
        //     by getaddrinfo().

	hints.ai_flags = AI_PASSIVE;
	//This field specifies additional options

	struct addrinfo *bind_address;
	getaddrinfo(0 , "8080", &hints, &bind_address);

	printf("Creating Socket...\n");
	SOCKET socket_listen;
	socket_listen = socket(bind_address->ai_family,bind_address->ai_socktype,bind_address->ai_protocol);
	//socket() creates an endpoint for communication and returns a file
	//descriptor that refers to that endpoint.  The file descriptor
        //returned by a successful call will be the lowest-numbered file
        //descriptor not currently open for the process.



	if(!ISVALIDSOCKET(socket_listen))
	{
		fprintf(stderr,"socket() failed (%d) \n",GETSOCKETERRNO());
		return 1;
	}

	printf("Binding socket to local address...\n");
	
	if(bind(socket_listen,bind_address->ai_addr,bind_address->ai_addrlen))//bind a name to a socket
	{
		fprintf(stderr,"bind() failed (%d) \n", GETSOCKETERRNO());
		return 1;
	}

	//When a socket is created with socket(2), it exists in a name space
        //(address family) but has no address assigned to it.  bind()
        //assigns the address specified by addr to the socket referred to by
        //the file descriptor sockfd.  addrlen specifies the size, in bytes,
        //of the address structure pointed to by addr.  Traditionally, this
        //operation is called “assigning a name to a socket”.

        //It is normally necessary to assign a local address using bind()
        //before a SOCK_STREAM socket may receive connections (see
        //accept(2))

	
	freeaddrinfo(bind_address);
	//The freeaddrinfo() function shall free one or more addrinfo structures returned by getaddrinfo(), along with any additional storage associated with those structures. 
	//If the ai_next field of the structure is not null, the entire list of structures shall be freed. 
	//The freeaddrinfo() function shall support the freeing of arbitrary sublists of an addrinfo list originally returned by getaddrinfo().

	printf("Listening...\n");
	
	if(listen(socket_listen,10)<0)//listen for connections on a socket
	{
		fprintf(stderr,"listen() failed. (%d) \n",GETSOCKETERRNO());
		return 1;
	}
	//listen() marks the socket referred to by sockfd as a passive
        //socket, that is, as a socket that will be used to accept incoming
        //connection requests using accept(2).

        //The sockfd argument is a file descriptor that refers to a socket
        //of type SOCK_STREAM or SOCK_SEQPACKET.

        //The backlog argument defines the maximum length to which the queue
        //of pending connections for sockfd may grow.  If a connection
        //request arrives when the queue is full, the client may receive an
        //error with an indication of ECONNREFUSED or, if the underlying
        //protocol supports retransmission, the request may be ignored so
        //that a later reattempt at connection succeeds.


	printf("Waiting for connection...\n");
	struct sockaddr_storage client_address;
	//A structure at least as large as any other sockaddr_* address structures. It's aligned so that a pointer to it can be cast 
	//as a pointer to other sockaddr_* structures and used to access its fields.
	
	socklen_t client_len = sizeof(client_address);
	
	SOCKET socket_client = accept(socket_listen, (struct sockaddr*) &client_address, &client_len);//accept a connection on a socket
	
	//The accept() system call is used with connection-based socket
        //types (SOCK_STREAM, SOCK_SEQPACKET).  It extracts the first
        //connection request on the queue of pending connections for the
        //listening socket, sockfd, creates a new connected socket, and
        //returns a new file descriptor referring to that socket.  The newly
        //created socket is not in the listening state.  The original socket
        //sockfd is unaffected by this call.

        //The argument sockfd is a socket that has been created with
        //socket(2), bound to a local address with bind(2), and is listening
        //for connections after a listen(2).

        //The argument addr is a pointer to a sockaddr structure.  This
        //structure is filled in with the address of the peer socket, as
        //known to the communications layer.  The exact format of the
        //address returned addr is determined by the socket's address family
        //(see socket(2) and the respective protocol man pages).  When addr
        //is NULL, nothing is filled in; in this case, addrlen is not used,
        //and should also be NULL.

        //The addrlen argument is a value-result argument: the caller must
        //initialize it to contain the size (in bytes) of the structure
        //pointed to by addr; on return it will contain the actual size of
        //the peer address.

        //The returned address is truncated if the buffer provided is too
        //small; in this case, addrlen will return a value greater than was
        //supplied to the call.

        //If no pending connections are present on the queue, and the socket
        //is not marked as nonblocking, accept() blocks the caller until a
        //connection is present.  If the socket is marked nonblocking and no
        //pending connections are present on the queue, accept() fails with
        //the error EAGAIN or EWOULDBLOCK.

        //In order to be notified of incoming connections on a socket, you
        //can use select(2), poll(2), or epoll(7).  A readable event will be
        //delivered when a new connection is attempted and you may then call
        //accept() to get a socket for that connection.  Alternatively, you
        //can set the socket to deliver SIGIO when activity occurs on a
        //socket; see socket(7) for details.


	if(!ISVALIDSOCKET(socket_client)){

		fprintf(stderr,"accept() failed. (%d) \n",GETSOCKETERRNO());
		return 1;
	}

	printf("Client is connected...");
	char address_buffer[100];

	getnameinfo((struct sockaddr*)&client_address,client_len,address_buffer,sizeof(address_buffer),0,0,NI_NUMERICHOST);
	//address-to-name translation in protocol-independent manner
	//
	//The getnameinfo() function is the inverse of getaddrinfo(3): it
        //converts a socket address to a corresponding host and service, in
        //a protocol-independent manner.  It combines the functionality of
        //gethostbyaddr(3) and getservbyport(3), but unlike those functions,
        //getnameinfo() is reentrant and allows programs to eliminate
        //IPv4-versus-IPv6 dependencies.

        //The addr argument is a pointer to a generic socket address
        //structure (of type sockaddr_in or sockaddr_in6) of size addrlen
        //that holds the input IP address and port number.  The arguments
        //host and serv are pointers to caller-allocated buffers (of size
        //hostlen and servlen respectively) into which getnameinfo() places
        //null-terminated strings containing the host and service names
        //respectively.

        //The caller can specify that no hostname (or no service name) is
        //required by providing a NULL host (or serv) argument or a zero
        //hostlen (or servlen) argument.  However, at least one of hostname
        //or service name must be requested.

	printf("%s\n",address_buffer);

	printf("Reading request...\n");
	char request[1024];
	
	int bytes_received = recv(socket_client,request,1024,0);//receive a message from a socket
	printf("Received %d bytes.\n", bytes_received);

	printf("Sending Response...\n");
	const char *response = 
		"HTTP/1.1 200 OK\r\n"
		"Connection : close\r\n"
		"Content-Type: text/plain\r\n\r\n"
		"Local time is : ";

	int bytes_sent= send(socket_client, response,strlen(response),0);//send a message on a socket
	printf("Sent %d of %d bytes.\n",bytes_sent ,(int)strlen(response));

	time_t timer;
	time(&timer);
	char *time_msg = ctime(&timer);
	bytes_sent = send(socket_client,time_msg,strlen(time_msg),0);//send a message on a socket
	printf("Sent %d of %d bytes.\n",bytes_sent,(int)strlen(time_msg));

	printf("Closing Connection...\n");
	CLOSESOCKET(socket_client);


	printf("Closing listening Socket...\n");
	CLOSESOCKET(socket_listen);


#if defined(_WIN32)
	WSACleanup();
#endif
	printf("Finished.\n");
	return 0;

}


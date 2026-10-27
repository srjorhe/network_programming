/*tcp_serve_toupper.c*/

#include "chap03.h"
#include <ctype.h>
int main()
{

#if defined(_WIN32)
	WSADATA d;
	if(WSAStartup(MAKEWORD(2,2),&d))
	{
		fprintf(stderr, "Failed to initialize.\n");
		return 1;
	}
#endif






printf("Configuring local address...\n");
struct addrinfo hints;
memset(&hints,0,sizeof(hints));

hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE;

struct addrinfo *bind_address;
getaddrinfo(0,"8080",&hints,&bind_address);

printf("Creating socket...\n");
SOCKET socket_listen;
socket_listen = socket(bind_address->ai_family,bind_address->ai_socktype,bind_address->ai_protocol);

if(!ISVALIDSOCKET(socket_listen))
{
	fprintf(stderr,"socket() failed. (%d)\n",GETSOCKETERRNO());
	return 1;

}

printf("Binding socket to local address...\n");
if(bind(socket_listen,bind_address->ai_addr,bind_address->ai_addrlen))
{
	fprintf(stderr,"bind() failed. (%d) \n" ,GETSOCKETERRNO());
	return 1;
}
freeaddrinfo(bind_address);

printf("Listening ... \n)";
if(listen(socket_listen, 10) < 0)
{
	frintf(stderr,"listen() failed. (%d) \n" , GETSOCKETERRNO());
	return 1;

}

fd_set master;
FD_ZERO(&master);
FD_SET(socket_listen, &master);
SOCKET max_socket = socket_listen;

printf("Waiting for connections...\n");

while(1)
{
	fd_set reads;
	reads = master;
	if(select(max_socket+1,&reads , 0,0,0) < 0)
	{
		fprintf(stderr,"select() failed. (%d) \n", GETSOCKETERRNO());
		return 1;
	}_address);
	//continue;
	SOCKET i;
	for(i = 1; i <= max_socket; ++i)
	{
		if(FD_ISSET(i,&reads))
		{
			//Handle socket
			if(i == socket_listen)
			{
			struct sockaddr_storage client_address;
			socklen_t client_line = sizeof(client_address);
			//continue;
			}
		}
	}

}

}

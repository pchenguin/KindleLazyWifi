#include <iostream>

using namespace std;
#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT	 5150
#define MAXLINE 1024
#include "ScreenTouch.h"

int main(int argc, char* argv[])
{
    ScreenTouch Touch;
    if (argc >= 2)
    {
        if (std::string(argv[1]) == "-reverse")
        {
            Touch.SetReverse(true);
        }
    }

    if( Touch.Init()==false ) return 1;

    int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in servaddr, cliaddr;

	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	// Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	socklen_t len;
        int n;

	len = sizeof(cliaddr); //len is value/result
while(1){

	n = recvfrom(sockfd, (char *)buffer, MAXLINE,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr,
				&len);
	buffer[n] = '\0';
	if( strcmp( "Key1Press",buffer)==0 )
    {
        Touch.Act(key_next_page);
    }
    else if( strcmp( "Key2Press",buffer)==0 )
    {
        Touch.Act(key_prev_page);
    }
	printf("Client : %s\n", buffer);
}
    return 0;
}

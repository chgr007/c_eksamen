#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* Custom includes */
#include "./include/server.h"
#include "./include/http_utils.h"

/* www.eastwillsecurity.com/pg3401/test.html */

int main(int iArgC, char *pszArgV[]) {
   // int sockFd, iNumConnections = 1;
    // create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    // bind the socket to a port
    struct sockaddr_in saAddr;
    memset(&saAddr, 0, sizeof(saAddr));
    saAddr.sin_family = AF_INET;
    saAddr.sin_addr.s_addr = INADDR_ANY;
    saAddr.sin_port = htons(8080);

    if (bind(sockfd, (struct sockaddr*) &saAddr, sizeof(saAddr)) < 0)
    {
        perror("ERROR on binding");
        exit(1);
    }

    // listen for incoming connections
    listen(sockfd, 5);

    // accept an incoming connection
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    int sockAcceptedFd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
    if (sockAcceptedFd < 0)
    {
        perror("ERROR on accept");
        exit(1);
    }

    // read the incoming request
    char buffer[1024];
    memset(buffer, 0, 1024);
//    int n = read(sockAcceptedFd, buffer, 1023);
//    if (n < 0)
//    {
//        perror("ERROR reading from socket");
//        exit(1);
//    }

    char * szRequestLine = (char *) malloc(1024 * sizeof (char));
    char *szFileName = (char *) malloc(1024 * sizeof (char));

    ReadLine(sockAcceptedFd, szRequestLine);
    printf("RequestLine %s\n", szRequestLine);
    GetRequestedFile(szRequestLine, szFileName);
    printf("FileName %s\n", szFileName);


    // send a response back to the client
    const char *response = "HTTP/1.1 200 OK\r\n\r\n<html><body><h1>Hello, world!</h1></body></html>\r\n";
    int n = write(sockAcceptedFd, response, strlen(response));
    if (n < 0)
    {
        perror("ERROR writing to socket");
        exit(1);
    }

    // close the socket
    close(sockAcceptedFd);
    close(sockfd);

    return 0;
}

/* Opens a TCP socket connection and returns it. */
int ConnectToSocket(char *szHost, int sockFd) {
    /* Do a DNS-lookup on the host */
    struct hostent *pHost = gethostbyname(szHost);
    int iStatus = OK;
    struct in_addr **AddrList = {0};

    if (!pHost) {
        printf("ERROR fetching host");
        return ERROR;
    }

    if (pHost->h_addrtype != AF_INET) {
        printf("Didn't find IPv4 for the hostname. Support for IPv6 not implemented\n");
        return ERROR;
    }

    AddrList = (struct in_addr **) pHost->h_addr_list;

    struct sockaddr_in saAddr = {0};
    saAddr.sin_family = AF_INET;
    saAddr.sin_port = htons(PORT);
    saAddr.sin_addr.s_addr = AddrList[0]->s_addr;

    int iConnectionStatus = connect(sockFd, (struct sockaddr *) &saAddr, sizeof(saAddr));
    if (iConnectionStatus < 0) {
        printf("ERROR connecting\nStatus: %d\n", iConnectionStatus);
        iStatus = ERROR;
    } else
        printf("Connected to socket!\n");

    return iStatus;
}

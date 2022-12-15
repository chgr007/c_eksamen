#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

/* Custom includes */
#include "./include/server.h"
#include "./include/http_utils.h"

/* www.eastwillsecurity.com/pg3401/test.html */

int main(int iArgC, char *pszArgV[]) {
    int sockFd, iNumConnections = 1;
    struct sockaddr_in saAddr = {0};

    saAddr.sin_family = AF_INET;
    saAddr.sin_port = htons(80);
    saAddr.sin_addr.s_addr = AF_LOCAL;

    if (sockFd = socket(AF_INET, SOCK_STREAM, 0) < 0) {
        printf("ERROR opening socket\n");
        return 1;
    }
    if (bind(sockFd, (struct sockaddr *)&saAddr, sizeof(saAddr)) < 0) {
        printf("ERROR on binding socket\n");
        return 1;
    }

    if (listen(sockFd, iNumConnections) < 0) {
        printf("ERROR on listen\n");
        return 1;
    }

    // TODO: Refactor this shit
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    if (accept(sockFd, (struct sockaddr*) &cli_addr, &clilen) < 0) {
        printf("ERROR on accept\n");
        return 1;
    }

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

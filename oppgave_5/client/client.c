#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

/* Custom includes */
#include "./include/client.h"
#include "./include/http_utils.h"

/* www.eastwillsecurity.com/pg3401/test.html */

int main(int iArgC, char *pszArgV[]) {
    int sockFd;

    if (iArgC < 2) {
        printf("Please specify a website to visit as an argument\n");
        return 0;
    }
    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        printf("ERROR opening socket\n");
        return 0;
    }
    struct URL *structURL = ParseURL(pszArgV[1]);

    int iConnectionStatus = ConnectToSocket(structURL->szHost, sockFd);
    if (iConnectionStatus != OK) {
        return 0;
    }

    int iRequestStatus = SendMessage(sockFd, structURL);
    if (iRequestStatus != OK) {
        return 0;
    }

    struct HTTP_RESPONSE *structHttpResponse = GetHeaders(sockFd);
    if(structHttpResponse == NULL) {
        return 0;
    }

    int iPayloadStatus = GetPayload(structHttpResponse, sockFd);
    if (iPayloadStatus == OK) {
        printf("%s\n", structHttpResponse->szPayload);
        return 1;
    }
    free(structURL);
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

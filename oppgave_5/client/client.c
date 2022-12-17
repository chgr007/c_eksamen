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
        printf("Please specify a filename as argument\n");
        return 0;
    }
    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        printf("ERROR opening socket\n");
        return 0;
    }

    struct URL *structURL = (struct URL *) malloc(sizeof(struct URL));
    memset(structURL, 0, sizeof(struct URL));
    strcpy(structURL->szHost, "localhost");
    printf("Path: %s\n", pszArgV[1]);
    strcpy(structURL->szPath, pszArgV[1]);
    printf("Host and path: %s/%s\n", structURL->szHost, structURL->szPath);
    strcpy(structURL->szProtocol, "HTTP/1.1");

    int iConnectionStatus = ConnectToSocket(structURL->szHost, sockFd);

    if (iConnectionStatus != OK) {
        printf("ERROR connecting to socket\n");
        return 0;
    }

    int iRequestStatus = SendMessage(sockFd, structURL);
    if (iRequestStatus != OK) {
        printf("ERROR sending message\n");
        return 0;
    }

    printf("Getting headers\n");
    struct HTTP_RESPONSE *structHttpResponse = GetHeaders(sockFd);
    if(structHttpResponse == NULL) {
        printf("ERROR getting headers\n");
        return 0;
    }
    printf("Getting payload\n");
    int iPayloadStatus = GetPayload(structHttpResponse, sockFd);
    if (iPayloadStatus == OK) {
        printf("%s\n", structHttpResponse->szPayload);
        SavePayload(structHttpResponse);
        return 1;
    }
    free(structURL);
    return 0;
}

/* Opens a TCP socket connection and returns it. */
int ConnectToSocket(char *szHost, int sockFd) {
    int iStatus = OK;
    struct sockaddr_in saAddr = {0};
    saAddr.sin_family = AF_INET;
    saAddr.sin_port = htons(PORT);
    saAddr.sin_addr.s_addr = INADDR_ANY;

    int iConnectionStatus = connect(sockFd, (struct sockaddr *) &saAddr, sizeof(saAddr));
    if (iConnectionStatus < 0) {
        printf("ERROR connecting\nStatus: %d\n", iConnectionStatus);
        iStatus = ERROR;
    } else
        printf("Connected to socket!\n");

    return iStatus;
}

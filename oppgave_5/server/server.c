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
    char *szRequestLine = (char *) malloc(1024 * sizeof(char));
    FILE_REQ *structFileReq = (FILE_REQ *) malloc(sizeof(FILE_REQ));

    struct sockaddr_in cli_addr;
    char buffer[1024];
    const char *response = "HTTP/1.1 200 OK\r\n\r\n<html><body><h1>Hello, world!</h1></body></html>\r\n";

    int sockFd;
    if ((sockFd = BindAndListen()) > -1) {
        // accept an incoming connection
        socklen_t clilen = sizeof(cli_addr);
        int sockAcceptedFd = accept(sockFd, (struct sockaddr *) &cli_addr, &clilen);
        if (sockAcceptedFd < 0) {
            perror("ERROR on accept");
            exit(1);
        }

        // read the incoming request
        memset(buffer, 0, 1024);

        ReadLine(sockAcceptedFd, szRequestLine);
        printf("RequestLine %s\n", szRequestLine);
        ParseFileRequest(szRequestLine, structFileReq);
        printf("FileName %s\n", structFileReq->szFilePath);
        // Get the file extension

        /* Check if we got a supported text format, otherwise read the file as binary */
        char *szFileReadOption = malloc(sizeof(char) * 3);

        bzero(szFileReadOption, 2);
        if (szFileExtension == HTML || szFileExtension == TXT || szFileExtension == C || szFileExtension == H ) {
            strcpy(szFileReadOption, "r");
        } else {
            strcpy(szFileReadOption, "rb");
        }

        FILE *fdFile = fopen(szFileName, szFileReadOption);

        if (fdFile != NULL) {
            printf("Found the file: %s\n", szFileName);
            // send a response back to the client
            int n = write(sockAcceptedFd, response, strlen(response));
            if (n < 0) {
                perror("ERROR writing to socket");
                exit(1);
            }
        } else {
            printf("File not found\n");
        }

        // close the socket
        close(sockAcceptedFd);
        close(sockFd);
        fclose(fdFile);
    }


    return 0;
}


int BindAndListen() {
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        perror("ERROR opening socket");
        return sockFd;
    }

    // bind the socket to a port
    struct sockaddr_in saAddr;
    memset(&saAddr, 0, sizeof(saAddr));
    saAddr.sin_family = AF_INET;
    saAddr.sin_addr.s_addr = INADDR_ANY;
    saAddr.sin_port = htons(8080);

    if (bind(sockFd, (struct sockaddr *) &saAddr, sizeof(saAddr)) < 0) {
        printf("ERROR on binding socket\n");
        close(sockFd);
        return -1;
    }

    // listen for incoming connections
    if(listen(sockFd, 5) < 0) {
        printf("ERROR on listening\n");
        close(sockFd);
        return -1;
    }

    return sockFd;
}

int GetFileExtension(char *szFileName) {
    char szTmp[256];

    if (szFileName == NULL || strlen(szFileName) == 0) {
        return ERROR;
    }

    strcpy(szTmp, szFileName);

    char *szTok = strtok(szTmp, ".");
    strtok(NULL, ".");

    if (szTok == NULL) {
        return ERROR;
    }

    if (strcmp(szTok, "html") == 0) {
        return HTML;
    } else if (strcmp(szTok, "txt") == 0) {
        return TXT;
    } else if (strcmp(szTok, "c") == 0) {
        return C;
    } else if (strcmp(szTok, "h") == 0) {
        return H;
    } else if(strcmp(szTok, "o") == 0) {
        return O;
    } else {
        return ERROR;
    }
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

#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include "include/http_utils.h"

struct HTTP_RESPONSE *GetHeaders(int sockFd) {
    char *szLineBuffer, *szToken;
    struct HTTP_RESPONSE *structHttpResponse;
    szLineBuffer = (char *) malloc(sizeof(char) * 256);
    structHttpResponse = malloc(sizeof(struct HTTP_RESPONSE));

    /* Get the first line / status line */
    ReadLine(sockFd, szLineBuffer);
    szToken = strtok(szLineBuffer, " ");
    strcpy(structHttpResponse->szVersion, szToken);
    szToken = strtok(NULL, " ");
    structHttpResponse->iStatusCode = atoi(szToken);
    /* Check if the first line is formatted as expected, could be more elaborated (could use RegEx if C has support for it) */
    if (structHttpResponse->iStatusCode == 0) {
        printf("ERROR: Unexpected format\n");
        return NULL;
    }
    szToken = strtok(NULL, " ");
    strcpy(structHttpResponse->szStatusMessage, szToken);

    /* Get header fields */
    SplitHeaders(szLineBuffer, structHttpResponse, sockFd);

    free(szLineBuffer);
    szLineBuffer = NULL;
    return structHttpResponse;
}

int ParseRequestHeaders(int sockFd, HTTP_REQUEST *structRequest) {
    char *szReqLine = (char *) malloc(sizeof(char) * 2048);
    /* Get the first line  */
    ReadLine(sockFd, szReqLine);
    ParseRequestLine(szReqLine, structRequest);
    free(szReqLine);
}


/* The function takes in a request line and returns the file/path to the szFileName pointer
 * The requested path / file resides in the middle of the request line with space as a delimiter. */
static int ParseRequestLine(char *szRequestLine, HTTP_REQUEST *structReq) {
    char *szToken;
    szToken = strtok(structReq, " ");

    // Crude validity check. TODO: Regex pattern matching here if time
    if (szToken == NULL) {
        printf("ERROR: Unexpected format\n");
        return ERROR;
    }

    strcpy(structReq->szMethod, szToken);
    szToken = strtok(NULL, " ");

    if (strlen(szToken) > 255) {
        printf("ERROR: Filename too long. Path and name must be less than 255 characters\n");
        return ERROR;
    }
    /* appending . so that it will be "./filename" (current folder). */
    strcpy(structReq->szFilePath, ".");
    strcat(structReq->szFilePath, szToken);

    if (!(structReq->szFileExt = GetFileExtension(structReq->szFilePath))) {
        return ERROR;
    }

    return OK;
}



int AcceptConnection(int serverSockFd) {
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    char *szRequestLine;
    char *szFileReadOption;
    FILE_REQ *structFileReq;

    int sockClientFd = accept(serverSockFd, (struct sockaddr *) &cli_addr, &clilen);
    const char *response = "HTTP/1.1 200 OK\r\n\r\n<html><body><h1>Hello, world!</h1></body></html>\r\n";
    printf("Inside loop, and accepted");

    if (sockClientFd > -1) {
        szRequestLine = (char *) malloc(1024 * sizeof(char));
        szFileReadOption = malloc(sizeof(char) * 3);
        structFileReq = (FILE_REQ *) malloc(sizeof(FILE_REQ));
        long lFileSize;

        bzero(szRequestLine, 1024);
        bzero(szFileReadOption, 3);

        // read the incoming request
        ReadLine(sockClientFd, szRequestLine);
        printf("RequestLine %s\n", szRequestLine);
        ParseFileRequest(szRequestLine, structFileReq);
        printf("FileName %s\n", structFileReq->szFilePath);
        // Get the file extension

        /* Check if we got a supported text format, otherwise read the file as binary */

        bzero(szFileReadOption, 3);
        if (structFileReq->szFileExt == HTML || structFileReq->szFileExt == TXT || structFileReq->szFileExt == C ||
            structFileReq->szFileExt == H) {
            strcpy(szFileReadOption, "r");
        } else {
            strcpy(szFileReadOption, "rb");
        }

        FILE *fdFile = fopen(structFileReq->szFilePath, szFileReadOption);

        if (fdFile != NULL) {
            printf("Found the file: %s\n", structFileReq->szFilePath);
            fseek(fdFile, 0L, SEEK_END);
            lFileSize = ftell(fdFile);
            fseek(fdFile, 0L, SEEK_SET);
            //WriteFileToSocket(fdFile, &sockClientFd, lFileSize);


            int iBytesRead;
            const char *responseHeader = "HTTP/1.1 200 OK\r\n\r\n";
            const char *response = "HTTP/1.1 200 OK\r\n\r\n<html><body><h1>Hello, world!</h1></body></html>\r\n";

            //iBytesRead = fread(byFileBuffer, 1, iFileSize, fdFile);
            printf("Got called!\n");
            send(sockClientFd, response, strlen(response), 0);



            // close the socket
            fclose(fdFile);
        } else {
            /* Write error message */
        }
    } else {
        printf("ERROR on accept\n");
        return 1;
    }
    close(sockClientFd);
}


int WriteFileToSocket(FILE *fdFile, int *sockClientFd, long iFileSize) {
    /* Size of an Ethernet frame */
    unsigned char byFileBuffer[iFileSize];
    int iBytesRead;
    const char *responseHeader = "HTTP/1.1 200 OK\r\n\r\n";
    const char *response = "HTTP/1.1 200 OK\r\n\r\n<html><body><h1>Hello, world!</h1></body></html>\r\n";

    //iBytesRead = fread(byFileBuffer, 1, iFileSize, fdFile);
    printf("Got called!\n");
    send(*sockClientFd, response, strlen(response), 0);
//    write(sockFd, responseHeader, strlen(responseHeader));
//    write(sockFd, byFileBuffer, iBytesRead);

//    while (!feof(fdFile)) {
//        iBytesRead = fread(byFileBuffer, 1, 1500, fdFile);
//
//        // Send 500 internal server error or something
//        if (ferror(fdFile)) {
//            printf("Error reading file\n");
//            fclose(fdFile);
//            close(sockFd);
//            break;
//        }
//        if (write(sockFd, byFileBuffer, iBytesRead) < 0) {
//            printf("Error writing to socket\n");
//            break;
//        }
//    }

}


/* Gets the header fields value and sets the HTTP_RESPONSE pointers fields accordingly */
int SplitHeaders(char *szLineBuffer, struct HTTP_RESPONSE *structHttpResponse, int sockFd) {
    char *szToken;
    bzero(szLineBuffer, 256);

    /* Loops through one by one line of the response, until it receives a blank NULL terminated line */
    ReadLine(sockFd, szLineBuffer);
    while (strcmp(szLineBuffer, "\0") != 0) {
        szToken = strtok(szLineBuffer, ": ");

        if (strcmp(szToken, "Server") == 0) {
            szToken = strtok(NULL, ": ");
            strcpy(structHttpResponse->szServer, szToken);
        } else if (strcmp(szToken, "Content-Type") == 0) {
            szToken = strtok(NULL, ": ");
            strcpy(structHttpResponse->szContentType, szToken);
        } else if (strcmp(szToken, "Content-Length") == 0) {
            szToken = strtok(NULL, ": ");
            structHttpResponse->iContentLength = atoi(szToken);
        }
        bzero(szLineBuffer, 256);
        ReadLine(sockFd, szLineBuffer);
    }
}

/* Reads one line of the response and updates a buffer */
int ReadLine(int sockFd, char *szLineBuffer) {
    char *szReceivedMessageBuffer = (char *) malloc(sizeof(char));
    bzero(szReceivedMessageBuffer, 1);

    while (TRUE) {
        int m = recv(sockFd, szReceivedMessageBuffer, 1, MSG_DONTWAIT);

        /* Lines are terminated with \r\n, so we have a new line at \n. We don't want to keep the \r neither, so we return \0 to find the blank line. */
        if (m >= 0) {
            if (*szReceivedMessageBuffer == '\n') {
                break;
            } else if (*szReceivedMessageBuffer != '\r') {
                strncat(szLineBuffer, szReceivedMessageBuffer, 1);
            }
            // strncat legger til \0
            //else strncat(szLineBuffer, "\0", 1);
        }
    }
    free(szReceivedMessageBuffer);
}

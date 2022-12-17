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

int SetResponseContentType(HTTP_REQUEST *structRequest) {
    char szTextPlain[] = "text/plain; charset=UTF-8";
    printf("fileExt: %d\n", structRequest->szFileExt);
    switch (structRequest->szFileExt) {
        case HTML:
            strcpy(structRequest->structHeaders->szContentType, "text/html; charset=UTF-8");
            break;
        case TXT:
            strcpy(structRequest->structHeaders->szContentType, szTextPlain);
            break;
        case C:
            strcpy(structRequest->structHeaders->szContentType, szTextPlain);
            break;
        case H:
            strcpy(structRequest->structHeaders->szContentType, szTextPlain);
            break;
        case O:
            strcpy(structRequest->structHeaders->szContentType, szTextPlain);
            break;
        case JPG:
            strcpy(structRequest->structHeaders->szContentType, "image/jpeg");
            break;
        case UNKNOWN:
            strcpy(structRequest->structHeaders->szContentType, szTextPlain);
            break;
        default:
            strcpy(structRequest->structHeaders->szContentType, szTextPlain);
            break;
    }
    printf("Set content type\n");
    return OK;
}

int ParseRequestHeaders(int sockFd, HTTP_REQUEST *structRequest) {
    char *szReqLine = (char *) malloc(sizeof(char) * 2048);
    /* Get the first line  */
    ReadLine(sockFd, szReqLine);

    if (ParseRequestLine(szReqLine, structRequest) != OK) {
        printf("Error parsing request line\n");
        free(szReqLine);
        return ERROR;
    }

    if ((structRequest->szFileExt = GetFileExtension(structRequest->szFilePath)) == -1) {
        free(szReqLine);
        return ERROR;
    }
    // print the fileExt
    printf("File extension: %d\n", structRequest->szFileExt);
    free(szReqLine);

    if (SplitHeaders(structRequest, sockFd) != OK) {
        return ERROR;
    }

    return OK;
}


int GetFileExtension(char *szFileName) {
    char szTmp[256];

    if (szFileName == NULL || strlen(szFileName) == 0) {
        return -1;
    }

    strcpy(szTmp, szFileName);

    char *szTok = strtok(szTmp, ".");
    strtok(NULL, ".");

    if (szTok == NULL) {
        return UNKNOWN;
    }

    if (strcmp(szTok, "html") == 0) {
        return HTML;
    } else if (strcmp(szTok, "txt") == 0) {
        return TXT;
    } else if (strcmp(szTok, "c") == 0) {
        return C;
    } else if (strcmp(szTok, "h") == 0) {
        return H;
    } else if (strcmp(szTok, "o") == 0) {
        return O;
    } else {
        return UNKNOWN;
    }
}

/* The function takes in a request line and returns the file/path to the szFileName pointer
 * The requested path / file resides in the middle of the request line with space as a delimiter. */

static int ParseRequestLine(char *szRequestLine, HTTP_REQUEST *structReq) {
    char *szToken;
    szToken = strtok(szRequestLine, " ");

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

    if ((structReq->szFileExt = GetFileExtension(structReq->szFilePath)) == -1) {
        printf("ERROR: File extension not supported\n");
        return ERROR;
    }

    return OK;
}



int AcceptConnection(int serverSockFd) {
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    char *szRequestLine;
    char *szFileReadOption;
    HTTP_REQUEST *structReq;

    int sockClientFd = accept(serverSockFd, (struct sockaddr *) &cli_addr, &clilen);
    const char *response = "HTTP/1.1 200 OK\r\n\r\n<html><body><h1>Hello, world!</h1></body></html>\r\n";
    printf("Accepted connection\n");

    if (sockClientFd > -1) {
        szRequestLine = (char *) malloc(1024 * sizeof(char));
        szFileReadOption = malloc(sizeof(char) * 3);
        structReq = (HTTP_REQUEST *) malloc(sizeof(HTTP_REQUEST));
        long lFileSize;

        bzero(szRequestLine, 1024);
        bzero(szFileReadOption, 3);
        memset(structReq, 0, sizeof(HTTP_REQUEST));
        printf("Reading request line\n");
        if (!(ParseRequestHeaders(sockClientFd, structReq))) {
            printf("ERROR: Could not parse request headers\n");
            close(sockClientFd);
            free(szRequestLine);
            free(szFileReadOption);
            free(structReq);
            return ERROR;
        }

        printf("Method: %s\n", structReq->szMethod);
        if (strcmp(structReq->szMethod, "GET") != 0) {
            printf("ERROR: Method not supported\n");
            close(sockClientFd);
            free(szRequestLine);
            free(szFileReadOption);
            free(structReq);
            return ERROR;
        }
        /* Check if we got a supported text format, otherwise read the file as binary */

        bzero(szFileReadOption, 3);
        if (structReq->szFileExt == HTML || structReq->szFileExt == TXT || structReq->szFileExt == C ||
            structReq->szFileExt == H) {
            strcpy(szFileReadOption, "r");
        } else {
            strcpy(szFileReadOption, "rb");
        }

        FILE *fdFile = fopen(structReq->szFilePath, szFileReadOption);

        if (fdFile != NULL) {
            printf("Found the file: %s\n", structReq->szFilePath);
            fseek(fdFile, 0L, SEEK_END);
            lFileSize = ftell(fdFile);
            fseek(fdFile, 0L, SEEK_SET);
            printf("File size: %ld\n", lFileSize);

            WriteFileToSocket(fdFile, sockClientFd, lFileSize);
            // close the socket
            fclose(fdFile);
        } else {
            /* Write error message */
        }
    } else {
        printf("ERROR on accept\n");
        return 1;
    }
    free(szRequestLine);
    free(szFileReadOption);
    free(structReq);
    close(sockClientFd);
}


int WriteFileToSocket(FILE *fdFile, int sockClientFd, long iFileSize) {
    /* Size of an Ethernet frame
       It might not be of any use to do it this way unless I write an algorithm to send a chunk of the file with the headers
       f it buffers 1500B on the network layer then this will be out of sync
    */
    char *byFileBuffer = malloc(sizeof (char) * 1500);
    memset(byFileBuffer, 0, 1500);
    int iBytesRead;
    const char *responseHeader = "HTTP/1.1 200 OK\r\n\r\n";
    //const char *response = "HTTP/1.1 200 OK\r\n\r\n<html><body><h1>Hello, world!</h1></body></html>\r\n";
    printf("Writing to socket!\n");
    //send(*sockClientFd, response, strlen(response), 0);
    write(sockClientFd, responseHeader, strlen(responseHeader));
//    write(sockFd, byFileBuffer, iBytesRead);

    while (!feof(fdFile)) {

        iBytesRead = fread(byFileBuffer, 1, 1500, fdFile);
        printf("Bytes read: %d\n", iBytesRead);
        // 500 internal server error or something. Got to read up on how to gracefully exit during transmission.
        // Guess this would be more of a network layer thing since the http-header is already sent?
        if (ferror(fdFile)) {
            printf("Error reading file\n");
            fclose(fdFile);
            return ERROR;
        }
        if (write(sockClientFd, byFileBuffer, iBytesRead) < 0) {
            printf("Error writing to socket\n");
            fclose(fdFile);
            return ERROR;
        } else {
            printf("Wrote to socket\n");
        }
    }

    printf("Done writing to socket!\n");
    return OK;
}


/* Gets the header fields value and sets the HTTP_RESPONSE pointers fields accordingly */
int SplitHeaders(HTTP_REQUEST *structHttpRequest, int sockFd) {
    char *szToken;
    char *szLineBuffer = (char *) malloc(1024 * sizeof(char));
    bzero(szLineBuffer, 1024);

    /* Loops through one by one line of the response, until it receives a blank NULL terminated line */
    ReadLine(sockFd, szLineBuffer);
    while (strcmp(szLineBuffer, "\0") != 0) {
        szToken = strtok(szLineBuffer, ": ");

        if (strcmp(szToken, "Content-Type") == 0) {
            szToken = strtok(NULL, ": ");
            strcpy(structHttpRequest->structHeaders->szContentType, szToken);
        } else if (strcmp(szToken, "Content-Length") == 0) {
            szToken = strtok(NULL, ": ");
            structHttpRequest->structHeaders->iContentLength = atoi(szToken);
        }
        bzero(szLineBuffer, 256);
        ReadLine(sockFd, szLineBuffer);
    }
    free(szLineBuffer);
    return OK;
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

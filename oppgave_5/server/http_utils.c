#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>

#include "include/http_utils.h"

struct HTTP_RESPONSE* GetHeaders(int sockFd) {
    char *szLineBuffer, *szToken;
    struct HTTP_RESPONSE *structHttpResponse;
    szLineBuffer = (char *) malloc(sizeof(char) * 256);
    structHttpResponse = malloc(sizeof(struct HTTP_RESPONSE));

    /* Get the first line / status line */
    ReadLine(sockFd, szLineBuffer);
    szToken = strtok(szLineBuffer,  " ");
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
        } else if (strcmp(szToken, "Content-Type")  == 0) {
            szToken = strtok(NULL, ": ");
            strcpy(structHttpResponse->szContentType, szToken);
        }
        else if(strcmp(szToken, "Content-Length")  == 0) {
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
            }
            else if (*szReceivedMessageBuffer != '\r') {
                strncat(szLineBuffer, szReceivedMessageBuffer, 1);
            }
            // strncat legger til \0
            //else strncat(szLineBuffer, "\0", 1);
        }
    }
    free(szReceivedMessageBuffer);
}

/* The function takes in a request line and returns the file/path to the szFileName pointer
 * The requested path / file resides in the middle of the request line with space as a delimiter. */
int ParseFileRequest(char *szReqLine, FILE_REQ *structFilReq) {
    char *szToken;
    szToken = strtok(szReqLine, " ");
    szToken = strtok(NULL, " ");
    if (szToken == NULL) {
        printf("ERROR: Unexpected format\n");
        return ERROR;
    }
    /* appending . so that it will be "./filename" (current folder). */
    if (strlen(szToken) > 255) {
        printf("ERROR: Filename too long. Path and name must be less than 255 characters\n");
        return ERROR;
    }
    strcpy(structFilReq->szFilePath, ".");
    strcat(structFilReq->szFilePath, szToken);

    if (!(structFilReq->szFileExt = GetFileExtension(structFilReq->szFilePath))) {
        return ERROR;
    }

    return OK;
}

int GetFile(char *szFilePath, char *szFileBuffer) {

}

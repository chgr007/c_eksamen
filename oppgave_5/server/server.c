#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
/* Custom includes */
#include "./include/server.h"
#include "./include/http_utils.h"

int iRunning = 1;
int sockFd;

void intHandler() {
    printf("\nClosing server socket\n");
    iRunning = 0;
    close(sockFd);
    exit(0);
}

int main(int iArgC, char *pszArgV[]) {
    signal(SIGINT, intHandler);

    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    char *szRequestLine;
    char *szFileReadOption;
    FILE_REQ *structFileReq;


    if ((sockFd = BindAndListen()) > -1) {
        // accept an incoming connection
        while(iRunning) {
            const char *response = "HTTP/1.1 200 OK\r\n\r\n<html><body><h1>Hello, world!</h1></body></html>\r\n";
            int sockClientFd = accept(sockFd, (struct sockaddr *) &cli_addr, &clilen);


//
//
//
//
            szRequestLine = (char *) malloc(1024 * sizeof(char));
            szFileReadOption = (char *) malloc(sizeof(char) * 3);
            structRequest = (HTTP_REQUEST *) malloc(sizeof(FILE_REQ));
            long lFileSize;
            bzero(szRequestLine, 1024);
            bzero(szFileReadOption, 3);
            /* TODO: Les hele jævla requesten */

            printf("Got called!\n");
            send(sockClientFd, response, strlen(response), 0);
//

//
//            printf("Got called!\n");
//            send(sockClientFd, response, strlen(response), 0);
//            // read the incoming request
//            ReadLine(sockClientFd, szRequestLine);
//            printf("RequestLine %s\n", szRequestLine);
//            ParseFileRequest(szRequestLine, structFileReq);
//            printf("FileName %s\n", structFileReq->szFilePath);
////            // Get the file extension
////
////            /* Check if we got a supported text format, otherwise read the file as binary */
////
//            if (structFileReq->szFileExt == HTML || structFileReq->szFileExt == TXT || structFileReq->szFileExt == C ||
//                structFileReq->szFileExt == H) {
//                strcpy(szFileReadOption, "r");
//            } else {
//                strcpy(szFileReadOption, "rb");
//            }



//            if (fdFile != NULL) {
//                printf("Found the file: %s\n", structFileReq->szFilePath);
//                fseek(fdFile, 0L, SEEK_END);
//                lFileSize = ftell(fdFile);
//                fseek(fdFile, 0L, SEEK_SET);
//                //WriteFileToSocket(fdFile, &sockClientFd, lFileSize);
//
//
//                int iBytesRead;
//                const char *responseHeader = "HTTP/1.1 200 OK\r\n\r\n";
//                const char *response = "HTTP/1.1 200 OK\r\n\r\n<html><body><h1>Hello, world!</h1></body></html>\r\n";
//
//                //iBytesRead = fread(byFileBuffer, 1, iFileSize, fdFile);
//                printf("Got called!\n");
//                send(sockClientFd, response, strlen(response), 0);
//                fclose(fdFile);
//
//
//                // close the socket
//            } else {
//                /* Write error message */
//            }
            close(sockClientFd);
        }
    }
    printf("Exiting...\n");
    return 0;
}

int BindAndListen() {
    struct sockaddr_in saAddr;
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        perror("ERROR opening socket");
        return sockFd;
    }

    // bind the socket to a port
    memset(&saAddr, 0, sizeof(saAddr));
    saAddr.sin_family = AF_INET;
    saAddr.sin_addr.s_addr = INADDR_ANY;
    saAddr.sin_port = htons(PORT);

    if (bind(sockFd, (struct sockaddr *) &saAddr, sizeof(saAddr)) < 0) {
        printf("ERROR on binding socket\n");
        close(sockFd);
        return -1;
    }

    // listen for incoming connections
    if (listen(sockFd, 5) < 0) {
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
    } else if (strcmp(szTok, "o") == 0) {
        return O;
    } else {
        return ERROR;
    }
}

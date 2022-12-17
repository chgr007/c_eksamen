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

void SIGINTHandler() {
    printf("\nClosing server socket\n");
    close(sockFd);
    iRunning = 0;
    exit(0);
}

void SIGABRTHandler() {
    close(sockFd);
    iRunning = 0;
    printf("\nERROR!\n");
    exit(1);
}

int main(int iArgC, char *pszArgV[]) {
    signal(SIGINT, SIGINTHandler);
    signal(SIGABRT, SIGABRTHandler);
    printf("Starting server on port: %d\nCTRL+C to quit\n", PORT);
    // TODO: Sjekk hva som skjer hvis man sender rare filnavn som typ ~ , . / osv.
    // Vil ikke at man skal få utilsiktet tilgang til root. Burde håndtere dette ved tid.

    if ((sockFd = BindAndListen()) > -1) {
        // accept an incoming connection
        while(iRunning) {
            struct sockaddr_in cli_addr;
            socklen_t clilen = sizeof(cli_addr);
            int sockClientFd = accept(sockFd, (struct sockaddr *) &cli_addr, &clilen);
            HandleConnection(sockClientFd);
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
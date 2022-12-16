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

    if ((sockFd = BindAndListen()) > -1) {
        // accept an incoming connection
        while(iRunning) {
            AcceptConnection(sockFd);
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


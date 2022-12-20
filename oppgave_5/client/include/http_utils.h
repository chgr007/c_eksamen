#ifndef C_PROG_HTTP_UTILS_H
#define C_PROG_HTTP_UTILS_H

#define PORT 8080
#define OK 1
#define ERROR 0
#define TRUE 1

struct HTTP_RESPONSE {
    int iStatusCode;
    long iContentLength;
    char szStatusMessage[56];
    char szVersion[56];
    char szContentType[56];
    char szServer[56];
    char *szPayload;
};

struct URL {
    char szHost[32];
    char szPath[512];
    char szProtocol[8];
};
/*
 * I'm using the iNumBytes (actual bytes read from the payload) just to be sure,
 * instead of relying on the Content-Length header.
 * */
int SavePayload(struct HTTP_RESPONSE *structHttpResponse, char *szFileName, int iNumBytes);
int ReadLine(int sockFd,char *szLineBuffer);
int SendMessage(int sockFd, struct URL *structUrl);
int SplitHeaders(char *szLineBuffer, struct HTTP_RESPONSE *structHttpResponse, int sockFd);
struct HTTP_RESPONSE* GetHeaders(int sockFd);
int GetPayload(struct HTTP_RESPONSE *structHttpResponse, int sockFd);

#endif //C_PROG_HTTP_UTILS_H

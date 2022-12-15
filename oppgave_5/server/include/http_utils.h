#ifndef C_PROG_HTTP_UTILS_H
#define C_PROG_HTTP_UTILS_H

#define PORT 80
#define OK 1
#define ERROR 0
#define TRUE 1

struct HTTP_RESPONSE {
    int iStatusCode;
    int iContentLength;
    char szStatusMessage[56];
    char szVersion[56];
    char szContentType[56];
    char szServer[56];
    char *szPayload;
};

struct HTTP_REQUEST_HEADERS {
    int iStatusCode;
    int iContentLength;
    char szFilePath[256];
    char szVersion[64];
    char szContentType[64];
    char *szPayload;
};

struct URL {
    char szHost[32];
    char szPath[512];
    char szProtocol[8];
};

enum FILE_TYPE {
    HTML,
    TXT,
    C,
    H,
    O
};

struct URL* ParseURL(char *szUrl);
int ReadLine(int sockFd,char *szLineBuffer);
int SendMessage(int sockFd, struct URL *structUrl);
int SplitHeaders(char *szLineBuffer, struct HTTP_RESPONSE *structHttpResponse, int sockFd);
struct HTTP_RESPONSE* GetHeaders(int sockFd);
int GetPayload(struct HTTP_RESPONSE *structHttpResponse, int sockFd);
int GetRequestedFile(char *szReqLine, char *szFileName);
int GetFileExtension(char *szFileName);
#endif //C_PROG_HTTP_UTILS_H

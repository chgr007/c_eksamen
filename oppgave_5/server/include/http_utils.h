#ifndef C_PROG_HTTP_UTILS_H
#define C_PROG_HTTP_UTILS_H

#define PORT 80
#define OK 1
#define ERROR 0
#define TRUE 1

enum FILE_TYPE {
    HTML,
    TXT,
    C,
    H,
    O
};

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

typedef struct _FILE_REQ {
    char szFilePath[256];
    enum FILE_TYPE szFileExt;
} FILE_REQ;


int WriteFileToSocket(FILE *fdFile, int sockFd, long iFileSize);

int BindAndListen();

struct URL *ParseURL(char *szUrl);

int ReadLine(int sockFd, char *szLineBuffer);

int ParseFileRequest(char *szReqLine, FILE_REQ *structFileReq);

int SplitHeaders(char *szLineBuffer, struct HTTP_RESPONSE *structHttpResponse, int sockFd);

int GetFile(char *szFilePath, char *szFileBuffer);

int GetFileExtension(char *szFileName);

#endif //C_PROG_HTTP_UTILS_H

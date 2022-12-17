#ifndef C_PROG_HTTP_UTILS_H
#define C_PROG_HTTP_UTILS_H

#define PORT 8080
#define OK 1
#define ERROR 0
#define TRUE 1

enum FILE_TYPE {
    HTML,
    TXT,
    C,
    H,
    O,
    JPG,
    UNKNOWN,
};

typedef struct _HTTP_RESPONSE_HEADERS {
    char szStatusMessage[56];
    char szVersion[56];
    char szContentType[56];
    char szServer[56];
    int iContentLength;
} HTTP_RESPONSE_HEADERS;

typedef struct _HTTP_RESPONSE {
    int iStatusCode;
    int iContentLength;
    char szStatusMessage[56];
    char szVersion[56];
    char szContentType[56];
    char szServer[56];
    HTTP_RESPONSE_HEADERS *structHeaders;
    char *szPayload;
} HTTP_RESPONSE;


struct HTTP_REQUEST_HEADERS {
    int iStatusCode;
    int iContentLength;
    char szContentType[128];
};

typedef struct _HTTP_REQUEST {
    char szFilePath[256];
    char szVersion[24];
    char szMethod[64];
    char *szPayload;
    enum FILE_TYPE szFileExt;
    struct HTTP_REQUEST_HEADERS *structHeaders;
} HTTP_REQUEST;

static int ParseRequestLine(char *szRequestLine, HTTP_REQUEST *structRequest);

int ParseRequestHeaders(int sockFd, HTTP_REQUEST *structRequest);

int WriteFileToSocket(FILE *fdFile, int sockFd, long iFileSize);

int AcceptConnection(int serverSockFd);

int BindAndListen();

struct URL *ParseURL(char *szUrl);

int ReadLine(int sockFd, char *szLineBuffer);

static int *GetHeaders(int sockFd, HTTP_REQUEST *structRequest);

int SplitHeaders(HTTP_REQUEST *structHttpRequest, int sockFd);

int GetFile(char *szFilePath, char *szFileBuffer);

int GetFileExtension(char *szFileName);

#endif //C_PROG_HTTP_UTILS_H

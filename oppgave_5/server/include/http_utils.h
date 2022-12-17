#ifndef C_PROG_HTTP_UTILS_H
#define C_PROG_HTTP_UTILS_H

#define PORT 8080
#define OK 1
#define ERROR 0
#define TRUE 1
#define SERVER "Server: Shaky C-Server/1.0\r\n"

enum FILE_TYPE {
    HTML,
    TXT,
    C,
    H,
    O,
    JPG,
    UNKNOWN,
};


typedef struct _HTTP_RESPONSE {
    int iStatusCode;
    char szContentLength[56];
    char szContentType[56];
    char szStatusMessage[56];
    char szVersion[56];
    char szServer[56];
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

static int ParseRequestHeaders(int sockFd, HTTP_REQUEST *structRequest);

static int WriteFileToSocket(FILE *fdFile, int sockFd, long iFileSize);

int HandleConnection(int sockClientFd);

int BindAndListen();

struct URL *ParseURL(char *szUrl);

static int ReadLine(int sockFd, char *szLineBuffer);

static int *GetHeaders(int sockFd, HTTP_REQUEST *structRequest);

static int SplitHeaders(HTTP_REQUEST *structHttpRequest, int sockFd);

static int GetFile(char *szFilePath, char *szFileBuffer);

static int GetFileExtension(char *szFileName);

static int SetResponseHeaders(HTTP_REQUEST *structRequest, HTTP_RESPONSE *structResponse, int iFileSize);
#endif //C_PROG_HTTP_UTILS_H

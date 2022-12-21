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
    char szContentLength[128];
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

/*
 * Takes an HTTP_REQUEST and an HTTP_RESPONSE as params
 *
 * Sets the content-type in the HTTP_RESPONSE based on the file extension in the HTTP_REQUEST
 *
 * Setting content type according to file format
 * */
static int SetResponseContentType(HTTP_REQUEST *structRequest, HTTP_RESPONSE *structResponse);

/* The function takes in a request line and returns the file/path to the szFileName pointer
 * The requested path / file resides in the middle of the request line with space as a delimiter.
 *
 * Returns OK on success and ERROR on failure.
 * */
static int ParseRequestLine(char *szRequestLine, HTTP_REQUEST *structRequest);

/*
 * Takes a client socket and HTTP_REQUEST as params
 *
 * Sets the headers and request line in the HTTP_REQUEST
 * Returns OK if successful, ERROR if not
 * */
static int ParseRequestHeaders(int sockFd, HTTP_REQUEST *structRequest);

/*
 * Takes a FILE, socket and fileSize as arguments
 *
 * Reads the file in chunks of 1500 bytes and writes it to the socket
 *
 * Returns OK on success, ERROR on failure
 */
static int WriteFileToSocket(FILE *fdFile, int sockFd, long iFileSize);

/*
 * Takes a client socket as parameter
 *
 * Runs for each accepted connection.
 * Handles and parses the requests, and acts accordingly.
 *
 * Returns OK on success, and ERROR on failure
 * */
int HandleConnection(int sockClientFd);


struct URL *ParseURL(char *szUrl);

/*
 * Takes the client socket and a string buffer as params.
 *
 * Reads one line of the request and sets the buffer with it
 *
 * Returns OK
 * */
static int ReadLine(int sockFd, char *szLineBuffer);

/*
 * Takes an HTTP_REQUEST as the first param and a socket file descriptor as the second param
 *
 * Parse lines for "key: val" and sets the HTTP_REQUEST struct accordingly
 *
 * Returns OK
 */
static int SplitHeaders(HTTP_REQUEST *structHttpRequest, int sockFd);

/*
 * Takes a client socket and a HTTP_RESPONSE as params
 *
 * Sends HTTP headers to the client socket
 *
 * Returns OK on success, and ERROR on failure
 * */
static int SendResponseHeaders(int sockFd, HTTP_RESPONSE *structResponse);

/*
 * Splits the string on the "." delimiter, and checks if the
 * file extension matches any known ones.
 * Returns an enum in the range of 0 - 6, where 6 is UNKNOWN.
 *
 * On error, it returns -1.
 */
static int GetFileExtension(char *szFileName);

/*
 * Set the header fields
 * in the HTTP_RESPONSE struct
 */
static int SetResponseHeaders(HTTP_REQUEST *structRequest, HTTP_RESPONSE *structResponse, long iFileSize);
#endif //C_PROG_HTTP_UTILS_H

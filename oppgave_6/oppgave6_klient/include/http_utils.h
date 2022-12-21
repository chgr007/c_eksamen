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
 * Takes an HTTP_RESPONSE, filename and file size as params
 * Using the iNumBytes (actual bytes read from the payload) just to be sure,
 * instead of relying on the Content-Length header.
 *
 * Saves the payload to a file with the szFileName
 *
 * Returns OK if successful, ERROR if not
 * */
int SavePayload(struct HTTP_RESPONSE *structHttpResponse, char *szFileName, int iNumBytes);

/*
 * Takes the socket and a string buffer as params.
 *
 * Reads one line of the request and sets the buffer with it
 *
 * Returns OK
 * */
int ReadLine(int sockFd,char *szLineBuffer);

/*
 * Takes a socket and a URL struct as params
 *
 * Send a request to an open socket.
 *
 * Returns OK on success and ERROR on failure
 * */
int SendMessage(int sockFd, struct URL *structUrl);
/*
 * Takes a string buffer, HTTP_RESPONSE struct and a socket as params
 *
 *
 * Parse lines for "key: val" and sets the HTTP_RESPONSE struct accordingly
 *
 * Returns OK
 */
int SplitHeaders(char *szLineBuffer, struct HTTP_RESPONSE *structHttpResponse, int sockFd);
/*
 * Parses the response from the server, and extracts the header
 *
 * The response expected from the server should be in the pattern of
 * HTTP/1.1 200 OK
 * Header-Field1: Header-Value1
 * Header-Field2: Header-Value2
 * Content-Length: n
 *
 * n size of payload
 * */
struct HTTP_RESPONSE* GetHeaders(int sockFd);
/*
 * Takes an HTTP_RESPONSE and a socket as params
 *
 * Reads the socket streams and sets the payload field in the HTTP_RESPONSE
 *
 * Returns OK on success and ERROR on failure
 * */
int GetPayload(struct HTTP_RESPONSE *structHttpResponse, int sockFd);
#endif //C_PROG_HTTP_UTILS_H

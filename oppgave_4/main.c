#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "include/oppgave4.h"

int main(int iArgC, char *pszArgV[]) {
    char pzPayload[] = "<html>"
                         "<body>"
                            "<p>Hello World!</p>"
                         "</body>"
                       "</html>";

    char httpReply[1024] = "HTTP/1.1 200 OK\r\n"
                      "Server: Apache/2.6.1.2.4.3.2\r\n"
                      "Content-Type: text/html; charset=UTF8\r\n";
    char pzContentLength[256];
    sprintf(pzContentLength, "Content-Length: %lu\r\n\r\n", strlen(pzPayload));

    strcat(httpReply, pzContentLength);
    strcat(httpReply, pzPayload);

    //printf("%s\n", httpReply);

    MYHTTP *pHttp = ProcessHttpHeader(httpReply);
    free(pHttp);
    return 0;
}
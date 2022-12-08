#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INPUT_FILE "./text_to_read.txt"
#define OUTPUT_FILE "./text_to_write.out.txt"

// Kode hentet fra min egen oppg. 6
int main() {
    long lFileBytes;
    char *sFileContent;

    // open a file called text_to_read.txt, return 1 if there's an error opening the file
    FILE *fdFile = fopen(INPUT_FILE, "r");
    if (fdFile == NULL) {
        printf("Error opening file");
        return 1;
    }
    // Rewind the cursor to the end of the file, count it's size and rewind it again. Then allocate
    // memory for the file content
    fseek(fdFile, 0L, SEEK_END);
    lFileBytes = ftell(fdFile);
    fseek(fdFile, 0L, SEEK_SET);
    sFileContent = (char *) malloc(lFileBytes * sizeof(char *));

    // Read the file content into the allocated memory
    fread(sFileContent, sizeof(char), lFileBytes, fdFile);
    // Close the file and free the memory
    fclose(fdFile);
    // Print the file content
    printf("%s", sFileContent);
    puts("\n");

    int i;
    char *szAsciiFormatedText = (char *) malloc((lFileBytes * sizeof(char *) * 2));
    bzero(szAsciiFormatedText, (lFileBytes * sizeof(char *) * 2));

    for (i = 0; i < strlen(sFileContent) - 1; i++) {

        int iCurrentChar = *(sFileContent + i);
        char *szHexValue = (char *) malloc(2 * sizeof(char));

        sprintf(szHexValue, "%X", iCurrentChar);

        printf("%c, %d, 0x%X", iCurrentChar, (char) *(sFileContent + i), iCurrentChar);
        strncat(szAsciiFormatedText, szHexValue, 2);
        puts("\n");
    }
    printf("Hex text: %s", szAsciiFormatedText);
    return 0;
}

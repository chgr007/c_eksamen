#ifndef C_EKSAMEN_MAIN_H
#define C_EKSAMEN_MAIN_H
#define INPUT_FILE "./text_to_read.txt"
#define OUTPUT_FILE "./text_to_write.txt"


/*
 * Takes a string of numbers, and converts it to a string of hex values
 * First param is the input string, second param is the output string
 *
 * Returns 1 if successful, 0 if not
 */

int ConvertStringToHex(char *szInputString, char *szOutputString);

/*
 * Takes a file pointer as argument and returns
 * the file size in bytes
 *
 * returns -1 on error
 * */
unsigned long GetFileSize(FILE *pFile);

/*
 * Opens a file, and reads the content into a string
 *
 * First param is the path, the second is a string to store the content in
 * On success it returns the length of the string, on failure it returns 0
 *
 */
unsigned long GetFileContent(FILE *fdFile, char *szFileContent, unsigned long ulFileSize);
/*
 * Takes a file as parameter, and writes its content as HEX in a new file.
 *
 * The file input and output is defined in the header file as INPUT_FILE and OUTPUT_FILE
 * */
int ConvertToHex(FILE *fdFile);
#endif //C_EKSAMEN_MAIN_H

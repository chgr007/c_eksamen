
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "include/beautifier.h"


int main(int iArgC, char *pszArgV[]) {

    if (iArgC < 2) {
        printf("ERROR: No file specified\n");
        printf("please run: %s filename.ext\n", pszArgV[0]);
        return 1;
    }
    StartFormatting(pszArgV[1]);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    char *fn1 = "input.txt";
    char *fn2 = "output.txt";

    FILE *fp1 = fopen(fn1, "r");
    if (!fp1) {
        perror("Input file error: ");
        return 1;
    }

    char lineBuf[1024];
    char *inBuf = malloc(sizeof(char)*1024);
    while (fgets(lineBuf, sizeof(lineBuf), fp1)) {
        if (strlen(inBuf) == sizeof(inBuf)) {
            char *newInBuf = malloc(sizeof(char)*strlen(inBuf)*2);
            strncpy(newInBuf, inBuf, strlen(inBuf));
            free(inBuf);
            inBuf = newInBuf;
        }
        strncat(inBuf, lineBuf, strlen(lineBuf));
    }

    FILE *fp2 = fopen(fn2, "w");
    if (!fp2) {
        perror("Output file error: ");
        return 1;
    }

    fwrite(inBuf, 1, (int)strlen(inBuf), fp2);

    fclose(fp1);
    fclose(fp2);

    return 0;
}
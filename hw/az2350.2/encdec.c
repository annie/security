#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hmacsha1.h"

char *PBKDF2(char *P, char *s, int c, int dkLen) {
    return "hi";
}

int sanitize(char input[], char *splitLine[], int maxArg) {
    int inDoubleQuotes = 0;
    int inSingleQuotes = 0;
    int lastDoubleQuotedStart = 0;
    int lastSingleQuotedStart = 0;
    int lastUnquotedStart = 0;

    int argCount = 0;

    char *start = input;

    int i = 0;
    while (i < strlen(input)) {
        // if we encounter an escape char
        if (input[i] == '\\' && i < strlen(input)-1) {
            if ((inDoubleQuotes == 1 || inSingleQuotes == 1)) {
                if (input[i+1] == '\\' || input[i+1] == '\'' || input[i+1] == '"') {
                    i += 2;
                    continue;
                }
                else {
                    // printf("input[i+1]: %c\n", input[i+1]);
                    printf("escape char error\n");
                    return 0;
                }
            }
        }
        // if we encounter a double quote
        if (input[i] == '"') {
            if (i >= 1 && input[i-1] != '\\') {
                // beginning of a double-quoted string
                // if (inDoubleQuotes == 0) {
                //     if (inSingleQuotes == 1) {
                //         continue;
                //     }
                // }
                if (inSingleQuotes == 0 && inDoubleQuotes == 0 && (input[i-1] == ' ' || i == strlen(input)-1)) {
                    inDoubleQuotes = 1;
                    lastDoubleQuotedStart = i;
                    i++;
                    continue;
                }
                if (inDoubleQuotes == 1) {
                    if (i < strlen(input) - 1 && input[i+1] != ' ') {
                        printf("unescaped double quote\n");
                        return 0;
                    }

                    // end of a double-quoted string
                    if (argCount > maxArg) {
                        printf("too many args\n");
                        return 0;
                    }

                    char *newArg = malloc(sizeof(char) * 256);
                    strncpy(newArg, start + lastDoubleQuotedStart, i - lastDoubleQuotedStart + 1);
                    int tail = i - lastDoubleQuotedStart + 1;
                    while (tail < strlen(newArg)) {
                        newArg[tail] = '\0';
                    }

                    splitLine[argCount] = newArg;
                    argCount++;
                    inDoubleQuotes = 0;
                }
            }
        }
        // if we encounter a single quote
        else if (input[i] == '\'') {
            if (i >= 1 && input[i-1] != '\\') {
                // beginning of a single-quoted string
                if (inDoubleQuotes == 0 && inSingleQuotes == 0 && (input[i-1] == ' ' || i == strlen(input)-1)) {
                    inSingleQuotes = 1;
                    lastSingleQuotedStart = i;
                    i++;
                    continue;
                }
                if (inSingleQuotes == 1) {
                    if (i < strlen(input) - 1 && input[i+1] != ' ') {
                        printf("unescaped single quote\n");
                        return 0;
                    }

                    // end of a single-quoted string
                    if (argCount > maxArg) {
                        printf("too many args\n");
                        return 0;
                    }

                    char *newArg = malloc(sizeof(char) * 256);
                    strncpy(newArg, start + lastSingleQuotedStart, i - lastSingleQuotedStart + 1);
                    int tail = i - lastSingleQuotedStart + 1;
                    while (tail < strlen(newArg)) {
                        newArg[tail] = '\0';
                    }
                    // printf("newArg: %s\n", newArg);
                    // printf("newArg[strlen(newArg)-1]: %c\n", newArg[strlen(newArg)-1]);
                    // printf("newArg2: %s\n", newArg);

                    splitLine[argCount] = newArg;
                    argCount++;
                    inSingleQuotes = 0;
                }
            }
        }
        // if we encounter a non-quote character that is succeeded by a space
        else if (i == strlen(input) - 1 || (input[i] != ' ' && input[i+1] == ' ')) {
            if (!inDoubleQuotes && !inSingleQuotes) {
                if (argCount > maxArg) {
                    printf("too many args");
                    return 0;
                }

                char *newArg = malloc(sizeof(char) * 256);
                strncpy(newArg, start + lastUnquotedStart, i - lastUnquotedStart + 2);
                // printf("UQ newArg[strlen(newArg)-1]:%cend\n", newArg[strlen(newArg)-1]);
                int tail = i - lastUnquotedStart + 2;
                while (tail < strlen(newArg)) {
                    newArg[tail] = '\0';
                }

                splitLine[argCount] = newArg;
                argCount++;
            }
        }

        // the beginning of an unquoted arg
        else if (i > 0 && input[i-1] == ' ' && !inDoubleQuotes && !inSingleQuotes) {
            lastUnquotedStart = i;
        }

        i++;
    }

    // printf("argcount: %d\n", argCount);

    // if (argCount != args) {
    //     return 0;
    // }

    if (inDoubleQuotes != 0 || inSingleQuotes != 0) {
        printf("unclosed quote\n");
        return 0;
    }
    return 1;
}

void strip(char arg[], char strippedArg[]) {
    int i = 0;
    int j = 0;
    int end = strlen(arg)-1;
    while (arg[end] == ' ' || arg[end] == '\0') {
        end--;
    }
    if (arg[0] == '"' || arg[0] == '\'') {
        i = 1;
        end--;
    }
    while (i <= end) {
        if (arg[i] == '\\' && i < end-1) {
            strippedArg[j] = arg[i+1];
            i += 2;
        }
        else {
            strippedArg[j] = arg[i];
            i++;
        }
        j++;
    }
    while (j < strlen(strippedArg)) {
        strippedArg[j] = '\0';
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        // start REPL
    }

    else {
        int i;
        for (i = 1; i < argc; i++) {

            FILE *fp = fopen(argv[i], "r");
            char line[256];

            if (!fp) {
                fprintf(stderr, "%s: file not found\n", argv[i]);
                continue;
            }

            char keyf[256];
            keyf[0] = '\0';

            while (fgets(line, sizeof(line), fp)) {
                char *splitLine[3] = { NULL };

                line[strlen(line)-1] = '\0';
                
                // printf("line: %s\n", line);
                // int sanitized = sanitize(line, splitLine, 2);
                // printf("sanitized: %d\n", sanitized);
                // printf("splitLine[0]: %s\n\n", splitLine[0]);

                int sanitized = sanitize(line, splitLine, 2);
                // printf("sanitized: %d\n", sanitized);
                // printf("first arg: %s\n", splitLine[0]);
                if (sanitized == 0 || !splitLine[0]) {
                    continue;
                }

                if (strncmp(splitLine[0], "encrypt", strlen("encrypt")) == 0) {
                    printf("splitLine[0]: %s\n", splitLine[0]);
                    printf("splitLine[1]: %s\n", splitLine[1]);
                    printf("splitLine[2]: %s\n", splitLine[2]);

                    if (strlen(keyf) == 0) {
                        fprintf(stderr, "keyfile must be specified before encrypt/decrypt commands\n");
                        continue;
                    }
                    if (!splitLine[1] || !splitLine[2]) {
                        fprintf(stderr, "encrypt must be called with 2 arguments: <inputfile> and <outputfile>\n");
                        continue;
                    }

                    char infile[256];
                    char outfile[256];
                    strip(splitLine[1], infile);
                    strip(splitLine[2], outfile);
                    printf("infile: %s\n", infile);
                    printf("outfile: %s\n", outfile);

                    int pid = fork();

                    // child process
                    if (pid == 0) {
                        char lastArg[256];
                        strncpy(lastArg, "file:", strlen("file:"));
                        strncat(lastArg, keyf, strlen(keyf));
                        execl("/usr/bin/openssl", "enc", "-aes-128-cbc", "-e", "-in", infile, "-out", outfile, "-pass", lastArg, (char *) NULL);
                    }

                    // check if fork failed
                    else if (pid < 0) {
                        fprintf(stderr, "fork failed\n");
                        return 1;
                    }
                }

                // else if (strncmp(splitLine[0], "decrypt", strlen("decrypt")) == 0) {
                //     // if (strlen(keyf) == 0) {
                //     //     fprintf(stderr, "keyfile must be specified before encrypt/decrypt commands");
                //     //     continue;
                //     // }
                //     // if (!splitLine[1] || !splitLine[2]) {
                //     //     fprintf(stderr, "decrypt is called with 2 arguments: <inputfile> and <outputfile>\n");
                //     //     continue;
                //     // }

                // }

                else if (strncmp(splitLine[0], "keyfile", strlen("keyfile")) == 0) {
                    char new_keyf[256];
                    strip(splitLine[1], new_keyf);

                    printf("new_keyf: %s\n", new_keyf);

                    fopen(new_keyf, "w");

                    strncpy(keyf, new_keyf, strlen(splitLine[1]));
                }

                else if (strncmp(splitLine[0], "password", strlen("password")) == 0) {
                    if (!splitLine[1] || !splitLine[2]) {
                        fprintf(stderr, "password is called with 2 arguments: <password> and <keyfile>");
                        continue;
                    }

                //     // char key[256];

                //     // hmac_sha1(splitLine[1], strlen(splitLine[1]), "salt", strlen("salt"), key);
                //     // // printf("key: %s\n", key);

                    printf("password: %s\n", splitLine[1]);
                    printf("pass keyfile: %s\n", splitLine[2]);

                    FILE *kfp = fopen(splitLine[2], "w");
                //     // fprintf(kfp, "%s", key);
                    fprintf(kfp, "%s", splitLine[1]);

                    strncpy(keyf, splitLine[2], strlen(splitLine[2]));
                }

                // else if (strncmp(splitLine[0], "cd", strlen("cd")) == 0) {
                    
                // }

                // else if (strncmp(splitLine[0], "mkdir", strlen("mkdir")) == 0) {
                    
                // }

                else {
                    fprintf(stderr, "%s: not a valid command\n", splitLine[0]);
                }
                int i;
                for (i = 0; i < 3; i++) {
                    if (splitLine[i]) {
                        free(splitLine[i]);
                    }
                }
            }

            fclose(fp);
        }
    }

    return 0;
}
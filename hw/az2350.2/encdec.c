#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <sys/types.h>
#include <sys/stat.h>

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
        if (input[i] == '\\') {
            if (inDoubleQuotes == 1) {
                if (input[i+1] == '\\' || input[i+1] == '"') {
                    i += 2;
                    continue;
                }
                else {
                    fprintf(stderr, "escape char error\n");
                    return 0;
                }
            }
            if (inSingleQuotes == 1) {
                if (input[i+1] == '\\' || input[i+1] == '\'') {
                    i += 2;
                    continue;
                }
                else {
                    fprintf(stderr, "escape char error\n");
                    return 0;
                }
            }
        }
        // if we encounter a double quote
        if (input[i] == '"') {
            if (i >= 1 && input[i-1] != '\\') {
                // beginning of a double-quoted string
                if (inSingleQuotes == 0 && inDoubleQuotes == 0 && input[i-1] == ' ') {
                    inDoubleQuotes = 1;
                    lastDoubleQuotedStart = i;
                    i++;
                    continue;
                }
                if (inDoubleQuotes == 1) {
                    if (i < strlen(input) - 1 && input[i+1] != ' ') {
                        fprintf(stderr, "unescaped double quote\n");
                        return 0;
                    }

                    // end of a double-quoted string
                    if (argCount > maxArg) {
                        fprintf(stderr, "too many args\n");
                        return 0;
                    }

                    char *newArg = malloc(sizeof(char) * 256);
                    strncpy(newArg, start + lastDoubleQuotedStart, i - lastDoubleQuotedStart + 1);
                    int tail = i - lastDoubleQuotedStart + 1;
                    while (tail < strlen(newArg)) {
                        newArg[tail] = '\0';
                        tail++;
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
                if (inDoubleQuotes == 0 && inSingleQuotes == 0 && input[i-1] == ' ') {
                    inSingleQuotes = 1;
                    lastSingleQuotedStart = i;
                    i++;
                    continue;
                }
                if (inSingleQuotes == 1) {
                    if (i < strlen(input) - 1 && input[i+1] != ' ') {
                        fprintf(stderr, "unescaped single quote\n");
                        return 0;
                    }

                    // end of a single-quoted string
                    if (argCount > maxArg) {
                        fprintf(stderr, "too many args\n");
                        return 0;
                    }

                    char *newArg = malloc(sizeof(char) * 256);
                    strncpy(newArg, start + lastSingleQuotedStart, i - lastSingleQuotedStart + 1);
                    int tail = i - lastSingleQuotedStart + 1;
                    while (tail < strlen(newArg)) {
                        newArg[tail] = '\0';
                        tail++;
                    }

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
                    fprintf(stderr, "too many args\n");
                    return 0;
                }

                char *newArg = malloc(sizeof(char) * 256);
                strncpy(newArg, start + lastUnquotedStart, i - lastUnquotedStart + 2);
                // printf("UQ newArg[strlen(newArg)-1]:%cend\n", newArg[strlen(newArg)-1]);
                int tail = i - lastUnquotedStart + 2;
                while (tail < strlen(newArg)) {
                    newArg[tail] = '\0';
                    tail++;
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

    if (inDoubleQuotes != 0) {
        fprintf(stderr, "unclosed double quote\n");
        return 0;
    }
    if (inSingleQuotes != 0) {
        fprintf(stderr, "unclosed single quote\n");
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
        // read from stdin
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

            static char keyf[256];
            keyf[0] = '\0';

            while (fgets(line, sizeof(line), fp)) {
                char *splitLine[3] = { NULL };

                int tail = strlen(line)-1;
                while (line[tail] == '\n' || line[tail] == ' ') {
                    line[tail] = '\0';
                    tail++;
                }

                int sanitized = sanitize(line, splitLine, 2);

                if (sanitized == 0 || !splitLine[0]) {
                    continue;
                }

                if (strncmp(splitLine[0], "encrypt", strlen("encrypt")) == 0) {

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

                    int pid = fork();

                    // child process
                    if (pid == 0) {
                        char lastArg[256] = { NULL };
                        strncpy(lastArg, "file:\0", strlen("file:\0"));
                        strncat(lastArg, keyf, strlen(keyf));
                        execl("/usr/bin/openssl", "enc", "-aes-128-cbc", "-base64", "-e", "-in", infile, "-out", outfile, "-pass", lastArg, (char *) NULL);
                    }

                    // check if fork failed
                    else if (pid < 0) {
                        fprintf(stderr, "fork failed\n");
                        return 1;
                    }
                }

                else if (strncmp(splitLine[0], "decrypt", strlen("decrypt")) == 0) {
                    if (strlen(keyf) == 0) {
                        fprintf(stderr, "keyfile must be specified before encrypt/decrypt commands");
                        continue;
                    }
                    if (!splitLine[1] || !splitLine[2]) {
                        fprintf(stderr, "decrypt is called with 2 arguments: <inputfile> and <outputfile>\n");
                        continue;
                    }

                    char infile[256];
                    char outfile[256];
                    strip(splitLine[1], infile);
                    strip(splitLine[2], outfile);

                    int pid = fork();

                    // child process
                    if (pid == 0) {
                        char lastArg[256] = { NULL };
                        strncpy(lastArg, "file:\0", strlen("file:\0"));
                        strncat(lastArg, keyf, strlen(keyf));
                        execl("/usr/bin/openssl", "enc", "-aes-128-cbc", "-base64", "-d", "-in", infile, "-out", outfile, "-pass", lastArg, (char *) NULL);
                    }

                    // check if fork failed
                    else if (pid < 0) {
                        fprintf(stderr, "fork failed\n");
                        return 1;
                    }

                }

                else if (strncmp(splitLine[0], "keyfile", strlen("keyfile")) == 0) {
                    if (!splitLine[1] || splitLine[2]) {
                        fprintf(stderr, "keyfile is called with 1 argument: <keyfile>");
                    }
                    char new_keyf[256];
                    strip(splitLine[1], new_keyf);

                    fopen(new_keyf, "w");

                    strncpy(keyf, new_keyf, strlen(new_keyf));

                }

                else if (strncmp(splitLine[0], "password", strlen("password")) == 0) {
                    if (!splitLine[1] || !splitLine[2]) {
                        fprintf(stderr, "password is called with 2 arguments: <password> and <keyfile>");
                        continue;
                    }

                    char new_password[256];
                    char new_keyf[256];

                    strip(splitLine[1], new_password);
                    strip(splitLine[2], new_keyf);

                    unsigned char keyBuf[8];

                    PKCS5_PBKDF2_HMAC_SHA1(new_password, strlen(new_password), NULL, 0, 1000, 8, keyBuf);

                    FILE *kfp = fopen(new_keyf, "w");
                    fprintf(kfp, "%s", keyBuf);

                    strncpy(keyf, new_keyf, strlen(new_keyf));
                }

                else if (strncmp(splitLine[0], "cd", strlen("cd")) == 0) {
                    if (!splitLine[1] || splitLine[2]) {
                        fprintf(stderr, "cd is called with 1 argument: <directory>");
                    }

                    char dir[256];
                    strip(splitLine[1], dir);

                    if (chdir(dir) == -1) {
                        fprintf(stderr, "failed to cd into directory %s\n", dir);
                        return 1;
                    }

                }

                else if (strncmp(splitLine[0], "mkdir", strlen("mkdir")) == 0) {
                    if (!splitLine[1] || splitLine[2]) {
                        fprintf(stderr, "mkdir is called with 1 argument: <directory>");
                    }

                    char dir[256];
                    strip(splitLine[1], dir);

                    if (mkdir(dir, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
                        fprintf(stderr, "failed to make directory %s\n", dir);
                        perror(0);
                        return 1;
                    }
                }

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
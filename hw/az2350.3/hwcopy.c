#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int getPathDepth(char *path) {
    int depth = 0;
    char *level = strtok(path, "/");
    while (level) {
        if (strncmp(level, "..", 2) == 0) {
            depth--;
        }
        else {
            depth++;
        }
        level = strtok(NULL, "/");
    }
    return depth;
}

int getFileNames(char *buf, char *fn1, char *fn2) {
    int i;
    int j = 0;
    int nameCount = 0;
    int spacePrev = 0;
    int spaceCount = 0;
    for (i = 0; i < strlen(buf); i++) {
        // read into fn1 until we encounter a space
        if (i == strlen(buf)-1 && buf[i] != ' ') {
            fn2[i-j] = buf[i];
            nameCount++;
        }
        else if (buf[i] == ' ') {
            if (spacePrev == 1) {
                return -1;
            }
            spacePrev = 1;
            spaceCount++;
        }
        else {
            if (spacePrev == 1) {
                nameCount++;
                if (nameCount > 2) {
                    return -1;
                }
                j = i;
                fn2[i-j] = buf[i];
                spacePrev = 0;
            }
            else if (spaceCount == 0) {
                fn1[i] = buf[i];
            }
            else if (spaceCount == 1) {
                fn2[i-j] = buf[i];
            }
            spacePrev = 0;
        }
        if (spaceCount > 1) {
            return -1;
        }
    }
    if (nameCount != 2) {
        return -1;
    }

    fn1[strlen(fn1)] = '\0';
    fn2[strlen(fn2)] = '\0';

    return 0;
}

int main(int argc, char **argv) {

    char *DEFAULT_CONFIG = "default_config.txt";

    if (argc != 1 && argc != 3) {
        fprintf(stderr, "usage: ./hwcopy OR ./hwcopy -c configfile\n");
        return 1;
    }

    // process config file
    FILE *cfp;

    // if no config file is specified, use default
    if (argc == 1) {
        cfp = fopen(DEFAULT_CONFIG, "r");
    }
    // if config file specified, use that file
    else {
        if (strncmp(argv[1], "-c", 2) != 0) {
            fprintf(stderr, "usage: ./hwcopy OR ./hwcopy -c configfile\n");
            return 1;
        }
        cfp = fopen(argv[2], "r");
    }

    if (!cfp) {
        fprintf(stderr, "error opening config file\n");
        return 1;
    }

    char line[1024];
    // int docRootNo;
    char *docRootBuf = malloc(sizeof(char)*1024);
    char *outAreaBuf = malloc(sizeof(char)*1024);
    // char *aliasBuf = malloc(sizeof(char)*1024);

    char *DOC_ROOT = "DocumentRoot";
    char *OUT_AREA = "OutputArea";
    char *ALIAS = "Alias";

    char *aliases[100];
    int aliasCount = 0;

    while (fgets(line, sizeof(line), cfp)) {
        char command[1024] = "";
        char path[1024] = "";
        char aliasPath[1024] = "";

        int command_len;
        int path_len;

        int index = 0;
        while (index < strlen(line) && line[index] != ' ') {
            command[index] = line[index];
            index++;
        }
        command[strlen(command)] = '\0';
        index++;
        command_len = index;
        while (index < strlen(line) && line[index] != ' ' && line[index] != '\n') {
            path[index-command_len] = line[index];
            index++;
        }
        path[strlen(path)] = '\0';
        index++;
        path_len = index;
        while (index < strlen(line) && line[index] != '\n') {
            aliasPath[index-path_len] = line[index];
            index++;
        }
        aliasPath[strlen(aliasPath)] = '\0';

        if (strncmp(command, DOC_ROOT, sizeof(char)*strlen(DOC_ROOT)) == 0) {
            if (strlen(docRootBuf) > 0) {
                fprintf(stderr, "you can only set DocumentRoot once in your config file!\n");
                return 1;
            }
            int path_index;
            for (path_index = 0; path_index <= strlen(path); path_index++) {
                if (path_index == strlen(path)) {
                    docRootBuf[path_index] = '\0';
                    break;
                }
                docRootBuf[path_index] = path[path_index];
            }
        }
        else if (strncmp(command, OUT_AREA, sizeof(char)*strlen(OUT_AREA)) == 0) {
            if (strlen(outAreaBuf) > 0) {
                fprintf(stderr, "you can only set OutputArea once in your config file!\n");
                return 1;
            }
            strncpy(outAreaBuf, path, strlen(path));
            outAreaBuf[strlen(outAreaBuf)] = '\0';
        }
        else if (strncmp(command, ALIAS, sizeof(char)*strlen(ALIAS)) == 0) {
            if (aliasCount == 100) {
                fprintf(stderr, "too many aliases in config file\n");
                return 1;
            }
            char fullAlias[1024] = "";
            int alias_index;
            for (alias_index = 0; alias_index < strlen(docRootBuf); alias_index++) {
                fullAlias[alias_index] = docRootBuf[alias_index];
            }
            for (alias_index = 0; alias_index < strlen(path); alias_index++) {
                if (alias_index == strlen(path)-1) {
                    fullAlias[alias_index + strlen(docRootBuf)] = '\0';
                    break;
                }
                fullAlias[alias_index + strlen(docRootBuf)] = path[alias_index];
            }

            // create symlink for alias (will not overwrite any previously set alias of the same name)
            aliases[aliasCount] = fullAlias;
            aliasCount++;

            symlink(aliasPath, fullAlias);
        }
        else {
            fprintf(stderr, "invalid line in config file: %s\n", line);
        }
    }
    fclose(cfp);

    if (strlen(docRootBuf) == 0) {
        fprintf(stderr, "no document root set in config file\n");
        return 1;
    }
    if (strlen(outAreaBuf) == 0) {
        fprintf(stderr, "no output area set in config file\n");
        return 1;
    }

    // read from stdin
    char buf[1024];

    while (fgets(buf, sizeof(buf), stdin)) {

        char *fn1 = malloc(sizeof(char)*1024);
        char *fn2 = malloc(sizeof(char)*1024);

        int i_fn1;
        int i_fn2;

        for (i_fn1 = 0; i_fn1 < 1024; i_fn1++) {
            fn1[i_fn1] = '\0';
        }

        for (i_fn2 = 0; i_fn2 < 1024; i_fn2++) {
            fn2[i_fn2] = '\0';
        }

        if (getFileNames(buf, fn1, fn2) < 0) {
            fprintf(stderr, "input must take the form <inputfile> <outfile>\n");
            continue;
        }

        char tmp_fn1[1024];
        strncpy(tmp_fn1, fn1, sizeof(char)*strlen(fn1));

        if (getPathDepth(tmp_fn1) < 0) {
            fprintf(stderr, "invalid input file path\n");
        }

        // append input filename to the document root specified in config file
        char *docRoot_fn1 = malloc(sizeof(char)*1024);

        int docRoot_index;
        for (docRoot_index = 0; docRoot_index <= strlen(docRootBuf); docRoot_index++) {
            if (docRoot_index == strlen(docRootBuf)) {
                docRoot_fn1[docRoot_index] = '\0';
                break;
            }
            docRoot_fn1[docRoot_index] = docRootBuf[docRoot_index];
        }

        if (fn1[0] != '/') {
            docRoot_fn1[strlen(docRoot_fn1)] = '/';
            docRoot_index++;
        }

        for ( ; docRoot_index <= strlen(docRootBuf) + strlen(fn1) + 1; docRoot_index++) {
            if (docRoot_index == strlen(docRootBuf) + strlen(fn1) + 1) {
                docRoot_fn1[docRoot_index] = '\0';
                break;
            }
            docRoot_fn1[docRoot_index] = fn1[docRoot_index - (strlen(docRootBuf)+1)];
        }

        // read input file into buffer
        FILE *fp1 = fopen(docRoot_fn1, "r");
        if (!fp1) {
            fprintf(stderr, "failed to open file %s\n", docRoot_fn1);
            continue;
        }

        char lineBuf[1024];
        char *inBuf = malloc(sizeof(char)*1024);
        while (fgets(lineBuf, sizeof(lineBuf), fp1)) {
            if (strlen(inBuf) == sizeof(inBuf)) {
                char *newInBuf = malloc(sizeof(char)*strlen(inBuf)*2);
                strncpy(newInBuf, inBuf, strlen(inBuf));
                free(inBuf);
                inBuf = newInBuf;
                free(newInBuf);
            }
            strncat(inBuf, lineBuf, sizeof(char)*strlen(lineBuf));
        }

        fclose(fp1);

        int pid = fork();

        if (pid == 0) {

            // write buffer to output file
            if (chroot(outAreaBuf) != 0) {
                perror("chroot error ");
                return 1;
            }

            if (fn2[0] != '/') {
                char *tmp_fn2 = malloc(sizeof(char)*1024);
                int tmp_fn2_index;
                for (tmp_fn2_index = 0; tmp_fn2_index <= strlen(fn2); tmp_fn2_index++) {
                    if (tmp_fn2_index == strlen(fn2)) {
                        tmp_fn2[tmp_fn2_index] = '\0';
                        break;
                    }
                    tmp_fn2[tmp_fn2_index] = fn2[tmp_fn2_index];
                }
                fn2[0] = '/';
                for (tmp_fn2_index = 1; tmp_fn2_index <= (strlen(fn2) + 1); tmp_fn2_index++) {
                    if (tmp_fn2_index == strlen(fn2) + 1) {
                        fn2[tmp_fn2_index] = '\0';
                        break;
                    }
                    fn2[tmp_fn2_index] = tmp_fn2[tmp_fn2_index-1];
                }
                free(tmp_fn2);
            }

            int z = strlen(fn2)-1;
            while (z < 1024) {
                fn2[z] = '\0';
                z++;
            }

            FILE *fp2 = fopen(fn2, "w");
            if (!fp2) {
                perror("output file error ");
                return 1;
            }

            fwrite(inBuf, 1, (int)strlen(inBuf), fp2);
            fclose(fp2);
            free(inBuf);

            exit(0);
        }

        free(fn1);
        free(fn2);
        free(docRoot_fn1);
    }

    int i;
    for (i = 0; i < aliasCount; i++) {
        unlink(aliases[i]);
    }

    free(docRootBuf);
    free(outAreaBuf);

    return 0;
}

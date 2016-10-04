#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

void getId(char *fid, char *name) {
    int i = 0;

    while (*fid != '_' && i < 255) {
        name[i] = *fid;
        fid++;
        i++;
    }

    name[i] = '\0';
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "correct usage: rmqueue <filename1>, <filename2>, ...\n");
        return 1;
    }

    char *dir_name = "queue";

    for (int i = 1; i < argc; i++) {
        // see if queue has file
        char path[4096];
        strcpy(path, dir_name);
        strcat(path, "/");
        strcat(path, argv[i]);

        struct stat fs;
        if (stat(path, &fs) != 0) {
            fprintf(stderr, "%s: file does not exist in queue\n", argv[i]);
            continue;
        }

        // get owner id of file
        char ownerId[11];
        getId(argv[i], ownerId);

        // compare to uid of caller
        if (geteuid() != atoi(ownerId)) {
            fprintf(stderr, "%s: you do not have permission to remove this file\n", argv[i]);
            continue;
        }

        // print success or failure message
        remove(path);
        printf("%s: removed file from queue", argv[i]);
    }

    return 0;
}
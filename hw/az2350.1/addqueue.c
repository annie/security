#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "addqueue.h"

// using djb2 hash function for arbitrary name (http://www.cse.yorku.ca/~oz/hash.html)
unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

char *setName(char *fname) {
    char new_fname[4096];
    char *fname_ptr = new_fname;

    // stores files in queue using the following name format: 
    // <user id>_<arbitrary hash name>_<timestamp>_<original filename>
    fname_ptr += sprintf(fname_ptr, "%lu", (unsigned long int)getuid());
    fname_ptr += sprintf(fname_ptr, "_%lu", hash(fname));
    fname_ptr += sprintf(fname_ptr, "_%lu", (unsigned int)time(NULL));
    fname_ptr += sprintf(fname_ptr, "_%s", fname);

    return new_fname;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "correct usage: addqueue <filename1>, <filename2>, ...\n");
        return 1;
    }

    // create new queue directory if it doesn't exist
    char *dir_name = "queue";
    mkdir(dir_name, S_IRWXU | S_IRWXG | S_IRWXO);
    
    int i = 1;
    for (i = 1; i < argc; i++) {
        
        char path[4096];
        strcpy(path, dir_name);
        strcat(path, "/");

        char *name = setName(argv[i]);
        strcat(path, name);

        FILE *fp = fopen(argv[i], "r");

        if (!fp) {
            fprintf(stderr, "%s: file not found\n", argv[i]);
            continue;
        }

        FILE *cp = fopen(path, "w");

        // get size of file
        fseek(fp, 0, SEEK_END);
        long buf_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        // read contents of file and store in buffer
        char *buf = malloc(sizeof(char) * (buf_size + 1));
        fread(buf, sizeof(char), buf_size, fp);
        if (ferror(fp)) {
            fprintf(stderr, "%s: error reading file\n", argv[i]);
            continue;
        }
        buf[buf_size] = "\0";

        // write contents of buffer to new file
        fwrite(buf, sizeof(char), buf_size, cp);
        
        fclose(fp);
        fclose(cp);
        free(buf);
    }

    return 0;
}

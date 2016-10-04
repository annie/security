#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>

int getName(char *fid, char *name) {
    int i = 0;

    while (*fid != '_' && i < 255) {
        name[i] = *fid;
        fid++;
        i++;
    }

    name[i] = '\0';
    return i + 1;
}

int main() {
    DIR *dir;
    struct dirent *dp;
    char *dir_name = "queue";

    dir = opendir(dir_name);

    if (dir == NULL) {
        fprintf(stderr, "error opening directory %s", dir_name);
        return 1;
    }

    while (dp = readdir(dir)) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        }

        char *fid = dp->d_name;
        char *f_ptr = fid;

        char arb_name[256];
        f_ptr += getName(f_ptr, arb_name);
        char uid[256];
        f_ptr += getName(f_ptr, uid);
        char timestamp[256];
        f_ptr += getName(f_ptr, timestamp);

        // convert UNIX timestamp to readable date/time
        char datetime[17];
        struct tm *dt;
        time_t ts = (atoi(timestamp));
        dt = localtime(&ts);
        strftime(datetime, sizeof(datetime), "%Y-%m-%d_%H:%M", dt);
        
        datetime[16] = '\0';

        printf("%s\t%s\t%s\t%s\n", arb_name, uid, datetime, fid);
    }

    closedir(dir);

    return 0;
}
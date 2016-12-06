#include <unistd.h>
#include <stdio.h>

int main() {
    char *path = "/Users/Annie/Documents/testDir/";
    // if (chroot(path) != 0) {
    //     perror("Error: ");
    //     return 1;
    // }
    chroot(path);
    FILE *fp = fopen("/test.txt", "w");
    char *text = "hello";
    fwrite(text, 1, sizeof(text), fp);
    fclose(fp);

    return 0;
}
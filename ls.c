#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

int is_dir(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}

int main() {
    char dirName[256];
    printf("Insert path: ");
    scanf("%s", dirName);

    char workingPath[256];


    DIR *d;
    struct dirent *dir;
    d = opendir(dirName);

    strcpy(workingPath, dirName);

    if (d) {
        while((dir = readdir(d)) != NULL) {
            printf("%s\n", dir->d_name);
            strcpy(workingPath, dirName);
            if (is_dir(strcat(workingPath, dir->d_name))) {
                printf("\tEs directorio\n");
            } else {
                printf("\tEs archivo\n");
            }
        }
        closedir(d);
    } else {
        printf("Error\n");
    }
    return 0;
}
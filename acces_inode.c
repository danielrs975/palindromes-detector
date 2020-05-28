#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>


int is_dir(const char* path){
    struct stat
}


main(int argc, char *argv[]){
    char *name = "./main.c";
    int fd;
    struct stat stbuf;

    stat(name, &stbuf);
    if (S_ISDIR(stbuf.st_mode) == 1){
        printf("soy un directorio\n");
    }
    else {
        printf("Soy un archivo\n");
    }

}
#include <stdio.h>

#include "../LibFS.h"
extern int osErrno;

/**
 * Exemple simple d'utilisation de la librairie FS
 * 
 * 
 * */

void usage(char *prog)
{
    fprintf(stderr, "usage: %s <disk image file>\n", prog);
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
	usage(argv[0]);
    }
    //le path sur le systeme hote de l image du disque
    char *path = argv[1];
    //bootrap du systeme de fichier est obligatoire
    FS_Boot(path);
    //En cas de creation d'une nouvelle image Sync doit
    //ecrire sur le disque
    FS_Sync();
    File_Create("/test3.txt")  ;
    int des = File_Open("/test3.txt")  ;
    File_Write(des, "GXXDLUCK",9);
    File_Seek(des, 1);
    File_Write(des, "OO",2);
    File_Close(des);
    FS_Sync();
    return 0;
}

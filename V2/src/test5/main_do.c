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
    Dir_Create("/dirtest3");
   // File_Create("/dirtest/test.txt")  ;
    FS_Sync();
printf("TAILLE : %d", Dir_Size("/"));
    return 0;
}

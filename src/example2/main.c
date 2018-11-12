#include <stdio.h>

#include "LibFS.h"
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

    if ( File_Create("/data.txt") == -1 )
    {
        fprintf(stderr, "erreur de File_Create\n");
        exit(1);
    }
    int fd = File_Open("/data.txt");
    if(fd == -1)
    {
        fprintf(stderr, "Erreur de File_Open\n");
        exit(1);
    }
    int nb = File_Write(fd,"Hello",6);
    if(nb != 5)
    {
        fprintf(stderr, "Erreur de File_Write\n");
        exit(1);
    }
    //ecrire sur le disque
    if( File_Close(fd) == -1)
    {
        fprintf(stderr, "Erreur de File_Close\n");
        exit(1);
    }
    FS_Sync();
    return 0;
}

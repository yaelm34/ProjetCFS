#include <stdio.h>
#include <string.h>

extern int osErrno;

#define OK write(1,"1\n",2);

#define K0 write(1,"0\n",2);

#include "../LibFS.h"
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
    
    //En cas de creatio d'une nouvelle image Sync doit
    //ecrire sur le disque
    FS_Sync();

    char buffer[1024];
    //
    int des = File_Open("/test3.txt")  ;
    int r  = File_Read(des, buffer,4);
    if( (r == 4) && (strncmp("GOOD",buffer,4) == 0) )
    {
        OK
    }

    r  = File_Read(des, buffer,4);
    if( (r == 4) && (strncmp("LUCK",buffer,4) == 0) )
    {
        OK
    }

    File_Close(des);
   

    //
    FS_Sync();
    return 0;
}

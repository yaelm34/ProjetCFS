#include <stdio.h>
#include <string.h>

#define OK write(1,"1\n",2);

#define K0 write(1,"0\n",2);

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
    
    //En cas de creatio d'une nouvelle image Sync doit
    //ecrire sur le disque
    FS_Sync();


    //

    char buffer[1024];

    int des = File_Open("/dirtest/test.txt")  ;
    if( des >= 0) 
    {
        int w  = File_Write(des, "QWERT",6);
        File_Close(des);
        des = File_Open("/dirtest/test.txt")  ;               
        int r  = File_Read(des, buffer,6);
        File_Close(des);
        if( strncmp("QWERT",buffer,5) == 0)
        {
            OK
        }
    }
    //

    //
    FS_Sync();
    return 0;
}

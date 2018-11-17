#include <stdio.h>

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

    

    int des = File_Open("/test4.txt")  ;
    if( (des >= 0) && File_Close(des) == 0) 
    {
        File_Unlink("/test4.txt");

        if( (File_Open("/test4.txt") < 0 ) && osErrno == E_NO_SUCH_FILE )
        {
            OK
        }
    }
    //

    //
    FS_Sync();
    return 0;
}

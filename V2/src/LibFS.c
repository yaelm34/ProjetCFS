#include "LibFS.h"
#include "Disque.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "suppl.c"
// variable errno pour gerer les erreurs
int osErrno;


//Imap inodeBitmap chargée
//Dmap DataBlocBitmap chargée 

#define magicNumber 0x7896E


typedef unsigned char byte;

typedef struct superbloc{

	byte unused[508]; //PADDING
	int magic_number; //MAGIC NUMBER

}superbloc;

typedef struct inode_bitmap{

	byte map[1024];

}inode_bitmap;


typedef struct databloc_bitmap{

	byte map[1024];

}databloc_bitmap;


typedef struct t_inode_bloc { 
    int tf;  //type fichier, 0 data, 1 repertoire
    int sz;  // taille en octets
    int adr[30];  // adresses des blocs
    } inode_bloc;

typedef struct data_bloc{ //structure pour créer un databloc 

	byte data[512];

}data_bloc;


typedef struct paire{ //32 octets

	char padding[12];
	char filename[16];
	int inode;

}paire;

static inode_bloc open_file_table[256]; //table des fichiers ouverts

static char* path_disk;

paire readPaire(int indice_bloc, int indice_paire){ //renvoie une paire à partir de l'indice 

	paire tab_paires[16];

	if(Disk_Read(2053+indice_bloc,(char*)tab_paires)==-1){

		osErrno=E_GENERAL;
		printf("Erreur: impossible de lire la table d'association");
		exit(-1);

	}

	return tab_paires[indice_paire];

}

int nameToInodeAux(char* path, int inode_depart, int indice_path){


	if(indice_path+1==strlen(path)){

		return inode_depart;

	}else{


		int indice_bloc=readinode(inode_depart).adr[0];

		
		if(readinode(inode_depart).tf==1){ //si c'est un dossier

			for(int i=0; i<16; i++){

				if(strcmp(readPaire(indice_bloc,i).filename,(char*)&path[indice_path])==0){

					nameToInodeAux(path,readPaire(indice_bloc,i).inode,indice_path+1);

				}


			}

		}
	

	}


	return -1;


}

int nameToInode(char* path){ //renvoie le numero d'inode depuis le nom du fichier


	return nameToInodeAux(path,0,0);

	 	char *string,*name;
 	char *res;
    string = strdup(path);
    printf("Original string: '%s'\n",string);

    int c=0;
    int i=c;

    while( (name = strsep(&string,"/")) != NULL && c<=1 ){
     
        res=name;
        c++;

    }

    printf("%s\n",res);

}


int create_root_inode(){
	
	inode inode;
	memset(&inode,-1,sizeof(inode));
	inode.tf=0;   //0=dossier 1=fichier
	inode.sz=0;
	inode.adr[0]=0;

	data_bloc data;
	memset(&data,-1,sizeof(data));

	Dmap[0]=0X01;
	savemaps();
	Disk_Write(5,(char*)&(inode));
	Disk_Write(2053,(char*)&(data));

	return 0;
	
	
}



int formatDisk(){ //formater le disque pour la première utilisation

	printf("Formatage du disque ...\n");


	superbloc v_superbloc;
	inode_bitmap v_inode_bitmap;
    databloc_bitmap v_databloc_bitmap;

	memset(&v_superbloc,0,sizeof(superbloc));
	v_superbloc.magic_number=magicNumber;
	memset(&v_inode_bitmap,0,sizeof(inode_bitmap));

	v_inode_bitmap.map[0]=0x01;

	memset(&v_databloc_bitmap,0,sizeof(databloc_bitmap));

	Disk_Write(0,(char*)&v_superbloc); //écrit le superblock au secteur 0
	Disk_Write(1,(char*)&(v_inode_bitmap.map));
	Disk_Write(2,(char*)&(v_inode_bitmap.map)+sizeof(Sector));
	Disk_Write(3,(char*)&(v_databloc_bitmap.map));
	Disk_Write(4,(char*)&(v_databloc_bitmap.map)+sizeof(Sector));


	create_root_inode();

	return 0;

		
}


int checkDisk(){ //Vérifier que le disque appartient bien au FS

	superbloc superbloc_read;

	Disk_Read(0,(char*)&superbloc_read);

	if(superbloc_read.magic_number==magicNumber){

		printf("FS reconnu \n");
		return 0;
	}


 	return -1;
	
	
}

int inodeWrite(inode_bloc inode, int inode_no){ //écrit un bloc d'inode dans la table des inodes

	int sec=inode_no/4; //indice du bloc
	int indice= inode_no % 4;//Indice interne
	inode_bloc sect_in[4]; //bloc de 4 inodes

	if(Disk_Read(5+sec,(char*)&(sect_in))==-1){  //récupération des inodes du secteur

		printf("Erreur: impossible d'enregistre le nouvel inode\n");
		osErrno=E_CREATE;
		return -1;
	}


	sect_in[indice]=inode; //ajout du nouvel inode


	if(Disk_Write(5+sec,(char*)&(sect_in))==-1){

		printf("Erreur: impossible d'enregistre le nouvel inode\n");
		osErrno=E_CREATE;
		return -1;
	}

	return 0;


	}





int FS_Boot(char *path){


	path_disk = path;

    printf("FS_Boot %s\n", path);

    memset(&open_file_table,0,sizeof(open_file_table)); //initialisation de la table des fichiers ouvert

    if (Disk_Init() == -1) {
	printf("Disk_Init() failed\n");
	osErrno = E_GENERAL;
	return -1;
    }
    

    if(Disk_Load(path)==-1){ //si le disque n'existe pas

		if(formatDisk()==-1){  //on le formate
			
			printf("formatDisk() failed\n"); //erreur de formatage
			osErrno = E_GENERAL;
			return -1;
			
			
		}

		if(FS_Sync(path)==-1){ //on le sauvegarde
				
				printf("FS_Sync() failed\n"); //erreur de sauvegarde du disque
				osErrno = E_GENERAL;
				return -1;
				
		}else{


			printf("Un nouveau disque a été créé et formaté\n");
		}
			
		
		
	}else{ //s'il existe
		
		if(checkDisk()==-1){ //vérification du FS
			
			printf("checkDisk() failed\n"); //erreur de vérification, le disque n'appartient pas à ce FS
			printf("Erreur: ce dique n'est pas compatible avec ce FS\n");
			osErrno = E_GENERAL;
			return -1;		
			
		}

		if(loadmaps()==-1){ //récup. des données bitmap du disque

			printf("Erreur: Impossible de lire les données \n");
			osErrno = E_GENERAL;
			return -1;		

		}else{

			printf("Les bitmaps ont été lues avec succès\n");
		}

	}


	printf("Système démarré avec succès\n");
	return 0;

}

int FS_Sync()
{

	printf("FS_Sync\n");
    
	if(Disk_Save(path_disk)==-1){

			printf("Erreur: Impossible de synchroniser le disque\n");
			osErrno = E_GENERAL;
			return -1;		

	
	}
    
	printf("Synchronisation effectuée avec succès\n");
    return 0;
}



int File_Create(char *file)
{
    printf("FS_Create\n");
	return 0;
 
}

int File_Open(char *file)
{
    printf("FS_Open\n");
    // TODO...
    return 0;
}

int File_Read(int fd, void *buffer, int size)
{
    printf("FS_Read\n");
    // TODO...
    return 0;
}

int File_Write(int fd, void *buffer, int size)
{
    printf("FS_Write\n");
    // TODO...
    return 0;
}

int File_Seek(int fd, int offset)
{
    printf("FS_Seek\n");
    // TODO...
    return 0;
}

int File_Close(int fd)
{
    printf("FS_Close\n");
    // TODO...
    return 0;
}

int File_Unlink(char *file)
{
    printf("FS_Unlink\n");
    // TODO...
    return 0;
}

char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; tmp = strstr(ins, rep); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}


// directory ops
int Dir_Create(char *path)
{
 
 	printf("FS_Dir_Create\n");

	//1 : récupération du nom : OK 

 	char *string,*name;
 	char *res;             //NOM DU DOSSIER A CREER
    string = strdup(path);
    printf("Original string: '%s'\n",string);

    while( (name = strsep(&string,"/")) != NULL){
     
        res=name;
      

    }

    printf("%s\n",res);

    char * slash = "/";

	char *rm = malloc(strlen(res)+strlen(slash)+1);
	strcpy(rm,slash);
	strcat(rm,res);
    printf("TEST %s\n",rm );
    char *string_rep =str_replace(path,rm,"");

    char *name2, *string2;
 	char *res_parent;             //NOM DU DOSSIER PARENT
    string2 = strdup(string_rep);
    printf("Original string: '%s'\n",string2);

    while( (name2 = strsep(&string2,"/")) != NULL){
     
        res_parent=name2;
      

    }

    printf("%s\n",res_parent);


    
    return 0;
}

int Dir_Size(char *path)
{

	//char name[50];
	//strcpy(name,strtok(path, "/"));

//	int indice_inode=nameToInode(name);

	printf("Dir_Size\n");

	int indice_inode=0;

	if(indice_inode==-1){

		osErrno=E_CREATE;
		printf("Erreur: Dir_Size() error\n");
		return -1;
	}

	int indice_bloc=readinode(indice_inode).adr[0];

	int cpt=0;

	for(int i=0; i<16; i++){

		if(readPaire(indice_bloc,i).inode!=-1){

			cpt++;
		}
	}


	return cpt;



}

int Dir_Read(char *path, void *buffer, int size)

{
   /*

   int inode = nameToInode(path);

   if(inode==-1){

   		osErrno=E_GENERAL;
   		printf("Erreur: Dir_Read() error\n");
   		return -1;

   }

   int indice_bloc=readinode(inode).adr[0];

   for(int i=0; i<16 ;i++){

   		paire pairei=readPaire(indice_bloc,i);
   		strcat(buffer,(char*)&pairei.filename);
   		strcat(buffer,(char*)&pairei.inode);


   }*/

   return 0;



}

int Dir_Unlink(char *path)
{
   /* printf("Dir_Unlink\n");

    int indice_inode=nameToInode(path);

    if(Dir_Size(path)!=0){

    	osErrno=E_DIR_NOT_EMPTY;
    	printf("Erreur: Le dossier n'est pas vide\n");
    	return -1;
    }

    if(indice_inode==0){

    	osErrno=E_ROOT_DIR;
    	printf("Erreur: Impossible de supprimer le répertoire racine\n");
    	return -1;
    }

    for(int i=0; i<30; i++){

	    int indice_bloc=readinode(indice_inode).adr[i];
	   	Dmap[indice_bloc]=0;
	

	 }

	   	Imap[indice_inode]=0;
	   	savemaps();*/

    return 0;
}


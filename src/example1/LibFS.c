#include "LibFS.h"
#include "Disque.h"
#include <stdio.h>
#include <string.h>
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


typedef struct inode_bloc{ //structure pour créer un inodebloc 

	int type; //0 dir 1 file
	int size;
	int pointers[30];


}inode_bloc;

typedef struct data_bloc{ //structure pour créer un databloc 

	byte data[512];

}data_bloc;


typedef struct association_table{

	byte table[8192];

}association_table;

static inode_bloc open_file_table[256]; //table des fichiers ouverts

static inode_bloc inode_bloc_table[8192]; //table des inodeblocs enregistrés

static data_bloc data_bloc_table[7947]; //table des datablocs enregistrés  



int loaddata(){ //charge les inodesblocs et datablocs du disque hôte 

	for(int i=5; i<2052; i++ ){ //inode table

		if (Disk_Read(i,(char*)&inode_bloc_table)+(128*(i-5))==-1) {

			osErrno=E_GENERAL;
			return -1;
		}
	}

	for(int i=2053; i<10000; i++){ //databloc table

		 if (Disk_Read(i,(char*)&data_bloc_table)+(512*(i-2053))==-1) {

			osErrno=E_GENERAL;
			return -1;
		}

	}

}


int savedata(){ //charge les inodesblocs et datablocs du disque hôte 

	for(int i=5; i<2052; i++ ){ //inode table

		if (Disk_Write(i,(char*)&inode_bloc_table)+(128*(i-5))==-1) {

			osErrno=E_GENERAL;
			return -1;
		}
	}

	for(int i=2053; i<10000; i++){ //databloc table

		 if (Disk_Write(i,(char*)&data_bloc_table)+(512*(i-2053))==-1) {

			osErrno=E_GENERAL;
			return -1;
		}

	}

}



int nameToInode(char* path){ //renvoie le numero d'inode depuis le nom du fichier

	return 0;

}


int create_root_inode(){
	
	inode_bloc inode;
	memset(&inode,-1,sizeof(inode));
	inode.type=0;   //0=dossier 1=fichier
	inode.size=0;
	inode.pointers[0]=0;



	inode_bloc_table[0]=inode;

	data_bloc data;
	memset(&data,-1,sizeof(data));

	data_bloc_table[0]=data;


	Disk_Write(5,(char*)&(inode_bloc_table));
	Disk_Write(2053,(char*)&(data_bloc_table));

	return 0;
	
	
}



int formatDisk(){ //formater le disque pour la première utilisation


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

	unsigned char magic_number_read;
	Disk_Read(0,(char*)&magic_number_read);

	if(magic_number_read==magicNumber){

		printf("FS reconnu");
		return 0;
	}


 	return -1;
	
	
}


int FS_Boot(char *path){
    printf("FS_Boot %s\n", path);

    memset(&open_file_table,0,sizeof(open_file_table));
    memset(&inode_bloc_table,0,sizeof(inode_bloc_table));
    memset(&data_bloc_table,0,sizeof(data_bloc_table));
    
    

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

		if(loadmaps()==-1||loaddata()==-1){ //récup. des données du disque

			printf("Erreur: Impossible de lire les données \n");
			osErrno = E_GENERAL;
			return -1;		

		}

	}


	printf("Système démarré avec succès\n");
	return 0;

}

int FS_Sync(path)
{

	printf("FS_Sync\n");
    
	if(Disk_Save(path)==-1){

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

    if(File_exists(file)==-1){


    }else{

    	printf("File_Create failed: file already exists\n");
    	osErrno=E_CREATE;
    	return -1;
    }

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


// directory ops
int Dir_Create(char *path)
{
    printf("Dir_Create %s\n", path);
    	//création du nouvel inode
	inode_bloc inode;
	memset(&inode,-1,sizeof(inode));
	inode.type=0;
	inode.size=0;


	int inode_no = findfree(Imap); //quel num d'inode dispo ?
	if (inode_no==-1){

		osErrno= E_CREATE;
		printf("Erreur: Impossible de créer le dossier\n");
		return -1;

	}

	inode_bloc_table[inode_no]=inode; //enregistrement de l'inode dans la table des inodes
	Imap[inode_no]=0x01; //définir l'inode comme étant occupé


	int bloc_no = findfree(Dmap); //quel num de bloc dispo ?
	if (bloc_no==-1){

		osErrno= E_CREATE;
		printf("Erreur: Impossible de créer le dossier\n");
		return -1;

	}

   	//création d'un data bloc

   	char *str=strtok(path,"/"); //récup. du nom

    int len= strlen(str);

   	int indice = len-2;
   	char name=str[indice];

   	if(strlen(name)>16){

   		osErrno=E_CREATE;
   		return -1;

   	}

	data_bloc bloc;
	strcpy(bloc.data,name); //copier le nom du dossier dans le bloc de données

	Dmap[bloc_no]=0x01; //définir le bloc comme étant occupé 

	if(savemaps()==-1 || FS_Sync()==-1){

		osErrno= E_CREATE;
		return -1;
	}


    return 0;
}

int Dir_Size(char *path)
{
    printf("Dir_Size\n");
    return 0;
}

int Dir_Read(char *path, void *buffer, int size)
{
    printf("Dir_Read\n");
    return 0;
}

int Dir_Unlink(char *path)
{
    printf("Dir_Unlink\n");
    return 0;
}

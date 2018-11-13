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

int nameToInode(char* path){ //renvoie le numero d'inode depuis le nom du fichier

	int inode_no;
	int l= strlen(path);

	for(int c=0; c<l-1 ;c++){

		for(int i=0; i<8192; i++){

			inode_no=i; 
			indice_bloc=readinode(inode_no).adr[0];
			int type=readinode(inode_no).tf;

			if(type==1){ //si c'est un dossier

				paire sect_paire[16];
				Disk_Read(2053+indice_bloc,(char*)sect_paire);

				for(int j=0; j<16; j++){

					if(sect_paire[j].filename=path[c]){

						if (c==l-1){

							
						}


					}else{

						return -1;
					}
				}
			}









		}
	}

	return 0;

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

     //création d'un data bloc

   	int bloc_no = findfree(Dmap); //quel num de bloc dispo ?

	if (bloc_no==-1){

		osErrno= E_CREATE;
		printf("Erreur: Impossible de créer le dossier: Out of range \n");
		return -1;

	}

   	char *str=strtok(path,"/"); //récup. du nom
    int len= strlen(str); //récup. du nom
   	char *name=str[len-1]; //récup. du nom
 
   	if(strlen(name)>16){

   		osErrno=E_CREATE;
   		printf("Erreur: Impossible de créer le dossier: Bad filename\n");
   		return -1;

   	}

	data_bloc bloc;
	memset(&bloc,0,sizeof(data_bloc));
	Disk_Write(2053+bloc_no,(char*)&bloc); //écriture du nouveau bloc


	Dmap[bloc_no]=0x01; //définir le bloc comme étant occupé 


    //création du nouvel inode

    inode_bloc inode;  
	memset(&inode,-1,sizeof(inode));
	inode.tf=0;
	inode.sz=0;
	inode.adr[0]=bloc_no;

	int inode_no = findfree(Imap); //quel num d'inode dispo ?

	if (inode_no==-1){

		osErrno= E_CREATE;
		printf("Erreur: Impossible de créer le dossier: Out of range\n");
		return -1;

	}

    if(inodeWrite(inode,inode_no)==-1){  //écrire l'inode au bon emplacement

    	osErrno=E_CREATE;
  		printf("Erreur:Impossible de créer le dossier: Write inode error\n");
  		return -1;

    } 

	Imap[inode_no]=0x01; //définir l'inode comme étant occupé

	paire ma_paire;

	strcpy(name,ma_paire.filename);
	ma_paire.inode=inode_no;

	//Chercher l'inode du répertoire parent

	char parent_name=str[len-2];
	int inode_parent_index=nameToInode(parent_name);


	int bloc_parent_index=readinode(inode_parent_index).adr[0];

	//placer ma_paire dans le répertoire parent

	byte data[512];

	Disk_Read(2053+bloc_parent_index,(char*)&data);

	char *new_data=strcat((char*)&data,(char*)&ma_paire);

	Disk_Write(2053+bloc_parent_index,new_data);	




	if(savemaps()==-1 || FS_Sync()==-1){ //sauvegarde des bitmaps mises à jour

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

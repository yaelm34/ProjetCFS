//
//quelques fonctions et conseils supplementaires
//
//  MM


 
// les bitmaps

static char Imap[1024];
static char Dmap[1024];


int readbit(char c, int pos) { // return 1, s'il y a un 1 sur la position pos dans c
/////////////////////////////////////////////////////////////////////////////
  char conv[]={0x80, 0x40,  0x20, 0x10,  0x08, 0x04, 0x02, 0x01};
  if (pos < 0 || pos > 7) {
	printf("bit: incorrect position \n");
	osErrno = E_GENERAL;
	return -1;
    }
    c = c & conv[pos]; 
    if (c == '\0')
    return 0;
    else
    return 1;
}

int setbit(char* c, int pos, int val) { // marquer val sur la position pos dans c
/////////////////////////////////////   //  val = 0  ou 1
  char conv[]={0x80, 0x40,  0x20, 0x10,  0x08, 0x04, 0x02, 0x01};
  char anticonv[]={0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD, 0xFE};
  if (pos < 0 || pos > 7) {
	printf("bit: incorrect position \n");
	osErrno = E_GENERAL;
	return -1;
    }
  if (val < 0 || val > 1) {
	printf("bit: incorrect value \n");
	osErrno = E_GENERAL;
	return -1;
    }
    if (val == 1)
        *c = (*c) | conv[pos];
    else
        *c = (*c) & anticonv[pos];
    return 0;
}


int  loadmaps()  // pour lire les bitmaps et les mettre dans les variables statiques
///////////////
{
   if ( (Disk_Read(1, Imap)  == -1) || (Disk_Read(2, Imap+512)  == -1) ) {
	printf("Disk_Read() Imap failed\n");
	osErrno = E_GENERAL;
	return -1;
    }
   if ( (Disk_Read(3, Dmap)  == -1) || (Disk_Read(4, Dmap+512)  == -1) ) {
	printf("Disk_Read() Dmap failed\n");
	osErrno = E_GENERAL;
	return -1;
    }
   return 0;
}

int  savemaps()  // pour sauvegarder les variables statiques sur le disque virtuel
///////////////
{
   if ( (Disk_Write(1, Imap)  == -1) || (Disk_Write(2, Imap+512)  == -1) ) {
	printf("Disk_Read() Imap failed\n");
	osErrno = E_GENERAL;
	return -1;
    }
   if ( (Disk_Write(3, Dmap)  == -1) || (Disk_Write(4, Dmap+512)  == -1) ) {
	printf("Disk_Read() Dmap failed\n");
	osErrno = E_GENERAL;
	return -1;
    }
   return 0;
}

int  readpos(char * M, int pos)  // retourne le bit M[pos]
///////////////////////////////  // valable pour les deux maps
{
  if (pos < 0 || pos > 8191) {
	printf("bit: incorrect position \n");
	osErrno = E_GENERAL;
	return -1;
    }
  int ind = pos / 8;
  int p = pos % 8;  
   return readbit(M[ind],p);
}


int  setpos(char * M, int pos, int val)  // change le bit M[pos]
///////////////////////////////////////  // valable pour les deux maps
{
  if (pos < 0 || pos > 8191) {
	printf("bit: incorrect position \n");
	osErrno = E_GENERAL;
	return -1;
    }
  if (val < 0 || val > 1) {
	printf("bit: incorrect value \n");
	osErrno = E_GENERAL;
	return -1;
    }
  int ind = pos / 8;
  int p = pos % 8;  
   return setbit(M+ind,p,val);
}

int  findfree(char * M)  // retourne l'indice du premier bit 0
///////////////////////  // valable pour les deux maps
{
  int i;
  for(i=0; i < 8192; i++) {
  if (readpos(M,i)==0)  break;
  }
  if (i== 8192) {
	printf("bitmap: full \n");
	osErrno = E_GENERAL;
	return -1;
    }  
   return i;
}


// type et acces pour les inodes

typedef struct t_inode { 
    int tf;  //type fichier, 0 data, 1 repertoire
    int sz;  // taille en octets
    int adr[30];  // adresses des blocs
    } inode;


    inode sect_in[4];  // un secteur de la table d'inodes
    char * pch = (char *) sect_in;  // si besoin

// on suppose qu'il y a 4 inodes par bloc et que la table d'inodes 
// commence au cinquieme bloc de la mémoire


inode  readinode( int I)  // retourne l'inode d'indice I
///////////////////////////////
{
  int ind = I / 4;  //indice du bloc
  int p = I % 4;    // indice interne
  inode sect_in[4];  //bloc d'inodes

   if  (Disk_Read(5+ind, (char *) sect_in)  == -1 ) {
	printf("Disk_Read() Itable failed\n");
	osErrno = E_GENERAL;
	exit( -1);
    }
  return sect_in[p];
}



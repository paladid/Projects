#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/sysmacros.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <signal.h>

//Az alábbi fügvénnyel kódoltuk be néhány byteba az "abc" stringet és ezáltal tudtuk tesztelni a program helyes működését.
 
/*char* TestArray(int *NumCh){
  char szo[4] = "abc";
  char* ptr = (char*)malloc(9);
  if(ptr == NULL){
    fprintf(stderr, "Error occured while allocating memory");
    exit(1);
  }
  for(int i = 0; i <= 8; i++){
    ptr[i] = rand() % 256;
  }

  for(int i = 0; i <= 2; i++){				//az egyes byteokra behelyezzük az "abc" string bitjeit
    ptr[i*3+0] = (ptr[i*3+0]& (~3)) + (szo[i]&3);
    ptr[i*3+1] = (ptr[i*3+1]& (~7)) + ((szo[i]&28)>>2);
    ptr[i*3+2] = (ptr[i*3+2]& (~7)) + ((szo[i]&224)>>5);
  }
  
  (*NumCh) = 9; //a memóriaterület nagyságának átadása

  return ptr;
}*/

//A függvény dinamikusan lefoglalja a szükséges mennyiségű memóriát a dekódolt szöveg számára majd a karaktereket behelyezi a megfelelő helyre.

char* Unwrap(char* Pbuff, int NumCh){

  char* ptr = (char*)malloc((NumCh)+1);

  if(ptr == NULL){
    fprintf(stderr, "Error occured while allocating memory");
    exit(1);
  }

  #pragma omp parallel for  //az összes processzor szálon egyszerre fog futni a folyamat
  for(int i = 0;i < NumCh ; i++){
    ptr[i] = (Pbuff[(i*3)]&3)<<6 | (Pbuff[(i*3)+1]&7)<<3 | (Pbuff[(i*3)+2]&7);  //leválasztjuk a biteket és behelyezzük a helyükre
  }

  ptr[(NumCh)] = '\0';  //megadjuk a zárókaraktert a stringnek

  free(Pbuff);

  return ptr;  //A szövegre mutató pointerrel térünk vissza
}

//A függvény beolvassa a bmp file tartalmát és a lefoglalt memória mutatójával tér vissza

char* ReadPixels(int f, int* NumCh){

  char buff[14]; //a file adatainak a beolvasása és eltárolása ez az első 14 byte
  read(f,buff,14);

  int filesize = ((int*)(buff+2))[0]; 
  int code = ((int*)(buff+2))[1];
  int data = ((int*)(buff+2))[2];

  char buff2[40]; //a file fejlécének a beolvasása és eltárolása
  read(f,buff2,40);

  char* ptr = (char*)malloc(filesize-54); //a pixelek helyének lefoglalása
  if(ptr == NULL){
    fprintf(stderr, "Error occured while allocating memory");
    exit(1);
  }

  read(f,ptr,filesize-54);

  *NumCh = code; //A kódolt karakterek számának visszaadása
  
  return ptr; 
}

//A file megnyitásáért felelős függvény

int BrowseForOpen(){

  char filename[50];

  struct stat inode;
  DIR *d;
  struct dirent *entry;

  chdir(getenv("HOME"));  //az alapértelmezett könyvtár megnyitása
  while(1){
    d = opendir(".");
    while((entry=readdir(d))!=NULL)
      {
        printf("%s\n",(entry)->d_name);
      }

    fflush(stdout); 

    scanf("%s",filename);

    struct stat filestat;

    if(stat(filename,&filestat)!=-1){		//ellenőrizzük, hogy létezik e a megadott név
      if((filestat.st_mode & S_IFMT)==S_IFDIR){  //ellenőrizzük, hogy mappa-e a megadott név
        chdir(filename);
      } else {
        break;
      }
    } else {
      printf("The name does not exist");
    }
  }

  int file = open(filename,O_RDONLY);

  return file;

  return 0;
}

//

int Post(char *neptunID, char *message, int NumCh){

  int sockfd;
  struct sockaddr_in servaddr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);		//socket létrehozása
  if (sockfd == -1) {
      printf("Socket creation failed\n");
      exit(1);
  }

  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("193.6.135.162");
  servaddr.sin_port = htons(80);
  connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

  char data[2048];

  strcat(data,"NeptunID=");
  strcat(data,neptunID);
  strcat(data,"&PostedText=");
  strcat(data,message);

  char inttostr[2048];

  sprintf(inttostr,"%d",(int)strlen(data));

  char msg[2048];

  strcat(msg,"POST /~vargai/post.php HTTP/1.1\r\n");			//Üzenet összefűzése
  strcat(msg,"Host: irh.inf.unideb.hu\r\n");
  strcat(msg,"Content-Length: ");
  strcat(msg,inttostr);
  strcat(msg,"\r\n");
  strcat(msg,"Content-Type: application/x-www-form-urlencoded\r\n");
  strcat(msg,"\r\n");
  strcat(msg,data);
                    
  send(sockfd,msg,strlen(msg)+1,0);

  char buff[2048];

  recv(sockfd,buff,sizeof(buff),0);

  close(sockfd);

  if(strstr(buff,"The message has been received.")==0){
    return 1;
  }

  return 0;
}

void WhatToDo(int sig){

  if(sig==SIGALRM){
    printf("Time out");
    exit(1);
  } 
  if(sig==SIGINT) {
    if(fork()==0){
      printf("Ctrl + c will not stop the program");
      fflush(stdout);
      kill(getpid(),SIGKILL);
    }
  }
}
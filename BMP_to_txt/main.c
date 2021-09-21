/*A program fordításához a "gcc" és a "-fopenmp" utasításokra lesz szükség tehát a programot a "gcc main.c -fopenmp -o main.elf" paranccsal tudjuk lefordítani
és a "./main.elf" segítságável tudjuk lefuttatni a programot*/

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
#include "functions.h"

int main(int argc, char* argv[]){

  char filename[100];
  int file;
  int pixellength;
  char* pixelbuff;
  char* decodebuff;
  char neptunID[7] = "VYTSWX";
  srand(time(0));

  if (argc <= 1){
    file = BrowseForOpen();
    if(file==-1){
      fprintf(stderr,"Error occured while opening the file");
      return 1;
    }
  } else if (strcmp(argv[1],"--version") == 0) {
    printf("Build: 1.0\nRelease: 2021.05.06\nAuthor: Paládi Sándor\n");
    return 0;
  } else if(strcmp(argv[1],"--help") == 0){
    printf("--version : megadja a program általános adatait\n--help : a lehetséges futtatási opciók\nMásodik argumentumként a file pontos nevének megadásával beolvashatjuk azt.");
    return 0;
  } else {
    strcpy(filename,argv[1]);
    file = open(filename,O_RDONLY);
    if(file==-1){
      fprintf(stderr,"Error occured while opening the file");
      return 1;
    }
  }

  signal(SIGALRM,WhatToDo);
  signal(SIGINT,WhatToDo);

  alarm(1);

  pixelbuff = ReadPixels(file,&pixellength);
  decodebuff = Unwrap(pixelbuff,pixellength);

  alarm(0);

  close(file);

  int post = Post(neptunID,decodebuff,pixellength);

  if(post == 0){
    printf("Message has been sent");
  } else {
    printf("Error occured in the sending process");
    return 1;
  }

  free(decodebuff);

  return 0;
}
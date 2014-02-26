#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"

int main(int argc, char *argv[]){
  int i,srid, rcid, *read_count,numofsrid;
  unsigned int *numofsr;
  struct StudentInfo *infoptr;
  int sema_set;
  FILE *output;

  
  srid = shmget(KEYSR, SEGSIZESR, 0);
  rcid = shmget(KEYRC, SEGSIZERC, 0);
  numofsrid = shmget(KEYNS, SEGSIZENS, 0);


  if(srid < 0 || rcid < 0 || numofsrid < 0){
    perror("create: shmget failed\n");
    exit(1);
  }

  infoptr = (struct StudentInfo *)shmat(srid,0,0);
  read_count = (int *)shmat(rcid,0,0);
  numofsr = (unsigned int *)shmat(numofsrid,0,0);

  if(infoptr <= (struct StudentInfo *)(0)||read_count <= (int *)(0)||numofsr <= (unsigned int *)(0)){
      perror("clean: shmget failed\n");
      exit(2);
    }

  sema_set = semget(SEMA_KEY,0,0);


  Wait(sema_set, 0);

  if((output=fopen("output.txt","w"))==NULL){
    printf("can't open file\n");
    exit(0);
  }

  for(i=0;i<*numofsr;i++){
    fputs(infoptr->Name,output);
    fprintf(output,"\n");
    fputs(infoptr->SID,output);
    fprintf(output,"\n");
    fputs(infoptr->Address,output);
    fprintf(output,"\n");
    fputs(infoptr->TelNum,output);
    fprintf(output,"\n");
    printf("%s\n",infoptr->Name);
    infoptr++;
  }

  printf("Clean Successfully and Information has been stored in output.txt\n");

  shmdt((char *)infoptr);
  shmctl(srid,IPC_RMID,(struct shmid_ds *)0);
  shmdt((char *)read_count);
  shmctl(rcid,IPC_RMID,(struct shmid_ds *)0);
  shmdt((char *)numofsr);
  shmctl(numofsrid,IPC_RMID,(struct shmid_ds *)0);
  sleep(10);
  Signal(sema_set,0);
  semctl(sema_set,0,IPC_RMID);
}

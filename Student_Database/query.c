#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/sem.h>
#include "header.h"

int main(int argc, char *argv[]){
  int i,srid, rcid, *read_count,numofsrid,found=0;
  unsigned int *numofsr;
  struct StudentInfo *infoptr;
  char *queryid;
  int sema_set;

  if(argc != 2){
    fprintf(stderr, "One Input Required\n");
    exit(3);
  }
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
      perror("query: shmget failed\n");
      exit(2);
    }

  sema_set = semget(SEMA_KEY,0,0);
  queryid = argv[1];
  printf("query: query id is %s\n",queryid);
 
  Wait(sema_set, 1);
  *read_count += 1;
  if(*read_count == 1)
    Wait(sema_set, 0);
  Signal(sema_set, 1);

  for(i=0;i<*numofsr;i++){
    if(!strcmp(queryid,infoptr->SID)){
      found = 1;
      printf("Student Record Found\n");
      printf("Name:%s\nSID:%s\nAddress:%s\nTelNum:%s\n",infoptr->Name,infoptr->SID,infoptr->Address,infoptr->TelNum);
      break;
    }
    infoptr++;
  }

  if(!found)
    printf("Student Record Not Found\n");
  sleep(10);

  Wait(sema_set,1);
  *read_count -= 1;
  if(*read_count == 0)
    Signal(sema_set,0);
  Signal(sema_set,1);
  return 0;
}

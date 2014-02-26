#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"

int deletenl(char *str){
  size_t n;
  n = strlen(str);
  if(str[n-1] == '\n')
    str[n-1]=0;
  return 0;
}


int main(int argc, char *argv[])
{
  int i,srid, rcid, *read_count, numofsrid;
  unsigned int *numofsr;
  struct StudentInfo *infoptr;
  int sema_set;
  FILE *datafile;

  if(argc != 2){
    fprintf(stderr, "One Input Required\n");
    exit(3);
  }

  srid = shmget(KEYSR, SEGSIZESR, IPC_CREAT|0666);/*Get shared memory of Student Record*/
  rcid = shmget(KEYRC, SEGSIZERC, IPC_CREAT|0666);/*Get shared memory of read_count*/
  numofsrid = shmget(KEYNS, SEGSIZENS, IPC_CREAT|0666);/* Get shared memory of no. of students*/
  
  if(srid <0 || rcid <0 || numofsrid<0 ){
    perror("create: shmget failed\n");
    exit(1);
  }

  infoptr = (struct StudentInfo *)shmat(srid,0,0);/*Attach shared memory of Student Record*/
  read_count = (int *)shmat(rcid,0,0);/* Attach shared memory of read_count*/
  numofsr = (int *)shmat(numofsrid,0,0);//Attach shared memory of numofsr
  sema_set = GetSemaphs(SEMA_KEY, NUM_SEMAPHS);/* Get a set of NUM_SEMAPHS semaphores */
  
  if(infoptr <= (struct StudentInfo *)(0)||read_count <= (int *)(0)||numofsr <= (unsigned int *)(0)){
      perror("load: shmget failed\n");
      exit(2);
    }


  if(sema_set < 0){
    perror("create: semget failed\n");
    exit(2);
  }

  datafile = fopen(argv[1], "r");/* open datafile */
  if(!datafile){
    printf("open file erro\n");
    return 0;
  }

  Wait(sema_set, 0);
  *read_count = 0;/*Initialize read_count to 0*/
  *numofsr = 0;
  while(!feof(datafile)){
    fgets(infoptr->Name,40,datafile);
    deletenl(infoptr->Name);
    printf("Information of %s has been stored in shared memory.\n",infoptr->Name);
    fgets(infoptr->SID,15,datafile);
    deletenl(infoptr->SID);
    fgets(infoptr->Address,100,datafile);
    deletenl(infoptr->Address);
    fgets(infoptr->TelNum,15,datafile);
    deletenl(infoptr->TelNum);
    *numofsr += 1;
    infoptr++;
  }
  *numofsr -= 1;
  printf("Num of Students = %d\n",*numofsr);
  sleep(10);
  Signal(sema_set,0);
  return 0;



}

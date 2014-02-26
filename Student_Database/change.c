#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/sem.h>
#include "header.h"


int deletenl(char *str){
  size_t n;
  n = strlen(str);
  if(str[n-1] == '\n')
    str[n-1]=0;
  return 0;
}



int main(int argc, char *argv[]){
  int i,srid, rcid, *read_count,numofsrid,found=0,choice;
  unsigned int *numofsr;
  struct StudentInfo *infoptr;
  char *changeid,password[10];
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
  changeid = argv[1];
  printf("change: change id is %s\n",changeid);

  printf("Password Needed:");
  scanf("%s",password);
  
  if(strcmp(password,"000")){
    printf("Invalid Password!\n");
    return 1;
  }

  Wait(sema_set, 0);

  for(i=0;i<*numofsr;i++){
    if(!strcmp(changeid,infoptr->SID)){
      found = 1;
      printf("Student Record Found,Here is the Information of the Student\n");
      printf("Name:%s\nSID:%s\nAddress:%s\nTelNum:%s\n",infoptr->Name,infoptr->SID,infoptr->Address,infoptr->TelNum);
      printf("Which one will you change?\n1.Name\n2.SID\n3.Address\n4.TelNum\n5.All\n");
      scanf("%d",&choice);
      getchar();
      switch(choice){
      case 1:
	printf("Please type a New Name:");
       	fgets(infoptr->Name,40,stdin);
	deletenl(infoptr->Name);
	break;
      case 2:
	printf("Please type a New SID\n");
	fgets(infoptr->SID,15,stdin);
	deletenl(infoptr->SID);
	break;
      case 3:
	printf("Please type a New Address\n");
	fgets(infoptr->Address,100,stdin);
	deletenl(infoptr->Address);
	break;
      case 4:
	printf("Please type a New TelNum\n");
	fgets(infoptr->TelNum,15,stdin);
	deletenl(infoptr->TelNum);
	break;
      case 5:
	printf("Please type Name,SID,Address,Telnum one by one\n");
	fgets(infoptr->Name,40,stdin);
	deletenl(infoptr->Name);
	fgets(infoptr->SID,15,stdin);
	deletenl(infoptr->SID);
	fgets(infoptr->Address,100,stdin);
	deletenl(infoptr->Address);
	fgets(infoptr->TelNum,15,stdin);
	deletenl(infoptr->TelNum);
	break;
      }
      break;
    }      
    infoptr++;
  }

  if(!found)
    printf("Student Record Not Found\n");
  sleep(10);
  Signal(sema_set,0);
  return 0;
}

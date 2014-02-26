#include "ssnfs.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>



#define BLOCKSIZE 512
#define NUM_FILE 10/*Max number of files per user*/
#define NUM_USER 10/*Max number of users*/
#define CAPACITY 20480/*Max number of blocks of the virtual disk, 20480*512 = 10MB*/
#define RES 64/*Reserved space for system information*/
#define NUM_BUNCH 2560/*Number of bunch(8 blocks) assigned, SHOULD BE CONSISTENT WITH CAPACITY
		       The first eight is occupied by system*/


struct paragraph{
  char words[8][BLOCKSIZE];
};
typedef struct paragraph paragraph;

struct filenode{
  char name[10];/*file_name*/
  unsigned int numofblock;/*Number of bunches: multiple of 8*/

  off_t position[8];/*indices of bunches of this file*/
  off_t endoffile;/*position of endoffile*/
};
typedef struct filenode filenode;

struct usernode{
  char name[10];/*user_name*/
  int numoffile;/*number of files owned by user*/
  filenode file[NUM_FILE];/*information of files owned by the user*/
};
typedef struct usernode usernode;


struct diskinfo{
  unsigned int numofuser;
  unsigned int isoccupied[NUM_BUNCH];
  unsigned int availbunch;/*Number of available bunches*/
  usernode user[NUM_USER];
};/*size of diskinfo is approximate 10 blocks*/
typedef struct diskinfo diskinfo;



create_output *create_file_1_svc(create_input *argp, struct svc_req *rqstp)
{
	static create_output output;
	FILE *vdisk; /*Virtual disk*/
	int fildes;/*file descriptor*/
	diskinfo vdiskinfo;
	paragraph content;
	char msg[100];

	char user_name[10], file_name[10];
	
	int i,j, userindex=0;
	int fileindex, bunchindex=-1,emptyindex=-1;

	/*Memory allocation for variables and initialization*/
	memset((create_output *)&output, 0,sizeof(create_output));
	output.out_msg.out_msg_val = malloc(100*sizeof(char));
	memset(output.out_msg.out_msg_val, 0,sizeof(char) * 100);

	
	memset((paragraph *)&content,0,sizeof(paragraph));


	strcpy(user_name,argp->user_name);strcpy(file_name,argp->file_name);

	printf("create function on server is invoked on server by %s\n", user_name);

	/*check the existence of virtual disk
	  if not, create and initialize
	 */
	if((vdisk=fopen("Virtual_Disk","r"))==NULL){
	  printf("Create Virtual Disk on Server and initialize it\n");
	  vdisk = fopen("Virtual_Disk","w");
	  fildes = open("Virtual_Disk",O_RDWR);

	  /*In address space, the first eight are occupied by system*/
	  memset((diskinfo *)&vdiskinfo,0,sizeof(diskinfo));
	  for(i=0;i<RES/8;i++)
	    vdiskinfo.isoccupied[i] = 1;
	  vdiskinfo.availbunch = NUM_BUNCH - RES/8;
	  vdiskinfo.numofuser = 0;
	  pwrite(fildes,&vdiskinfo,sizeof(diskinfo),0);
	  close(fildes);
	}
	fclose(vdisk);

        fildes = open("Virtual_Disk",O_RDWR);
	pread(fildes,&vdiskinfo,sizeof(diskinfo),0);
	//		printf("%s,%d,%d\n",vdiskinfo.user[0].name,0,vdiskinfo.numofuser);

	//		  printf("%ld\n",sizeof(diskinfo));
	
	//	printf("Create file on server\n");
	/*Check existence of user*/
	for(i=0;i<vdiskinfo.numofuser;i++){
	  if(!strcmp(vdiskinfo.user[i].name,user_name)){
	      userindex = i;
	      break;
	  }
	}
	if(i==vdiskinfo.numofuser){
	  userindex = vdiskinfo.numofuser;
	  vdiskinfo.numofuser = vdiskinfo.numofuser + 1;
	  vdiskinfo.user[userindex].numoffile = 0;
	  for(j=0;j<NUM_FILE;j++)
	    vdiskinfo.user[userindex].file[j].name[0] = '\0';
	  strcpy(vdiskinfo.user[userindex].name,user_name);
	}
	
	/*Check whether the file exists*/
	//	printf("Whether the file exists?\n");
	j = 0;
	for(i=0;i<NUM_FILE;i++){
	  if(j>=vdiskinfo.user[userindex].numoffile)break;
	  //	  printf("%d,%d\n",vdiskinfo.user[userindex].file[i].name[0]=='\0',i);
	  if(vdiskinfo.user[userindex].file[i].name[0]=='\0'){emptyindex=i;continue;}
	  j = j + 1;
	  if(!strcmp(vdiskinfo.user[userindex].file[i].name,file_name)){
	    strcpy(msg,"File already exists!");
	    strcpy(output.out_msg.out_msg_val,msg);
	    output.out_msg.out_msg_len = strlen(msg);
	    close(fildes);
	    return &output;
	  }
	}
	if(emptyindex==-1)emptyindex = i;
	//	printf("Traverse all files, ei=%d, j=%d,numoffile=%d\n",emptyindex,j,vdiskinfo.user[userindex].numoffile);
	

	/*Allocate space for new file, initial size: 8blocks*/
	/*Search for available free bunch*/
	for(i=0;i<NUM_BUNCH;i++)
	  if(vdiskinfo.isoccupied[i]==0){bunchindex = i;break;}

	if(bunchindex==-1){
	    strcpy(msg,"No available memory!");
	    strcpy(output.out_msg.out_msg_val,msg);
	    output.out_msg.out_msg_len = strlen(msg);
	    close(fildes);
	    return &output;
	}

	/*Create a file in user's directory*/
	if(j>=vdiskinfo.user[userindex].numoffile){
	  if(j>=NUM_FILE){
	    strcpy(msg,"REACH MAX NUM OF FILES! CAN'T NOT CREATE A NEW ONE!");
	    strcpy(output.out_msg.out_msg_val,msg);
	    output.out_msg.out_msg_len = strlen(msg);
	    close(fildes);
	    return &output;
	  }
	  fileindex = emptyindex;
	  vdiskinfo.user[userindex].numoffile = vdiskinfo.user[userindex].numoffile + 1;
	  vdiskinfo.availbunch--;
	  strcpy(vdiskinfo.user[userindex].file[fileindex].name,file_name);
	}

	//	printf("Create FILE successfully\n");

	pwrite(fildes,&content,sizeof(paragraph),bunchindex*BLOCKSIZE*8);
	//	printf("Create FILE successfully %d, %d,%d\n",userindex,fileindex,bunchindex);

	/*Save the information of the file*/
	vdiskinfo.user[userindex].file[fileindex].numofblock = 1;
	vdiskinfo.user[userindex].file[fileindex].position[0] = bunchindex;
	vdiskinfo.user[userindex].file[fileindex].endoffile = 0;
	vdiskinfo.isoccupied[bunchindex] = 1;

	pwrite(fildes,&vdiskinfo,sizeof(diskinfo),0);

	/*Test*/
	pread(fildes,&vdiskinfo,sizeof(diskinfo),0);
	//	printf("user: %s\n",vdiskinfo.user[userindex].name);
	//	printf("numofuser:%d,numoffile:%d\n",vdiskinfo.numofuser,vdiskinfo.user[userindex].numoffile);

	close(fildes);

	strcpy(msg,file_name);
	strcat(msg, " has been created for ");
	strcat(msg, user_name);
	strcpy(output.out_msg.out_msg_val,msg);
	output.out_msg.out_msg_len = strlen(msg);
	return &output;
}

list_output *list_files_1_svc(list_input *argp, struct svc_req *rqstp)
{
	static list_output  result;

	char user_name[10];

	FILE *vdisk; /*Virtual disk*/
	int fildes;/*file descriptor*/
	diskinfo vdiskinfo;

	char msg[200];
	
	int i,j , userindex=-1;


	printf("list function on server is invoked by  %s\n",argp->usrname);

	/*Memory allocation for variables and initialization*/
	memset((list_output *)&result, 0,sizeof(list_output));
	result.out_msg.out_msg_val = malloc(100*sizeof(char));
	memset(result.out_msg.out_msg_val, 0,sizeof(char) * 100);

	/*Get current username*/
	strcpy(user_name,argp->usrname);

	if((vdisk=fopen("Virtual_Disk","r"))==NULL){
	  strcpy(msg, "Virtual Disk not constructed yet. No files exist for ");
	  strcat(msg, user_name);
	  strcpy(result.out_msg.out_msg_val,msg);
	  result.out_msg.out_msg_len = strlen(msg);
	  return &result;
	}
	fclose(vdisk);
	
        fildes = open("Virtual_Disk",O_RDONLY);
	pread(fildes,&vdiskinfo,sizeof(diskinfo),0);
	close(fildes);

	/*Check existence of user*/
	for(i=0;i<vdiskinfo.numofuser;i++){
	  if(!strcmp(vdiskinfo.user[i].name,user_name)){
	      userindex = i;
	      break;
	  }
	}
	
	if(userindex==-1){
	  strcpy(msg, "No files exist for ");
	  strcat(msg, user_name);
	  strcpy(result.out_msg.out_msg_val,msg);
	  result.out_msg.out_msg_len = strlen(msg);
	  close(fildes);
	  return &result;
	}

	//	printf("%d,%s\n",userindex,vdiskinfo.user[userindex].name);

	/*List all files in user's directory*/
	strcpy(msg, "");

	/*Check whether the file exists*/
	j = 0;
	for(i=0;i<NUM_FILE;i++){
	  if(j>=vdiskinfo.user[userindex].numoffile)break;
	  //	  printf("%s,%d\n",vdiskinfo.user[userindex].file[i].name,i);
	  if(vdiskinfo.user[userindex].file[i].name[0]=='\0'){continue;}
	  j = j + 1;
	  if(j>1)strcat(msg, "\t");
	  strcat(msg, vdiskinfo.user[userindex].file[i].name);
	}
	close(fildes);
	strcpy(result.out_msg.out_msg_val,msg);
	result.out_msg.out_msg_len = strlen(msg);
	//	printf("%s\n",result.out_msg.out_msg_val);
	return &result;
}

delete_output *
delete_file_1_svc(delete_input *argp, struct svc_req *rqstp)
{
	static delete_output  result;	

	FILE *vdisk; /*Virtual disk*/
	int fildes;/*file descriptor*/
	diskinfo vdiskinfo;

	paragraph content;

	char msg[100];

	char user_name[10], file_name[10];
	
	int i,j,k, userindex=0;
	int fileindex,bunchindex;

	/*Memory allocation for variables and initialization*/
	memset((delete_output *)&result, 0,sizeof(delete_output));
	result.out_msg.out_msg_val = malloc(100*sizeof(char));
	memset(result.out_msg.out_msg_val, 0,sizeof(char) * 100);

	memset((paragraph *)&content,0,sizeof(paragraph));

	/*Get current username and filename*/
	strcpy(user_name,argp->user_name);
	strcpy(file_name,argp->file_name);

	printf("Delete function on server is invoked by %s!\n",user_name);

	if((vdisk=fopen("Virtual_Disk","r"))==NULL){
	  strcpy(msg, "Virtual Disk not constructed yet. No files exist for ");
	  strcat(msg, user_name);
	  strcpy(result.out_msg.out_msg_val,msg);
	  result.out_msg.out_msg_len = strlen(msg);
	  printf("%s\n",result.out_msg.out_msg_val);
	  return &result;
	}
	fclose(vdisk);
	
        fildes = open("Virtual_Disk",O_RDWR);
	pread(fildes,&vdiskinfo,sizeof(diskinfo),0);

	/*Check existence of user*/
	for(i=0;i<vdiskinfo.numofuser;i++){
	  if(!strcmp(vdiskinfo.user[i].name,user_name)){
	    userindex = i;
	    break;
	  }
	}
	
	if(userindex==-1){
	  strcpy(msg, "No files exist for ");
	  strcat(msg, user_name);
	  strcpy(result.out_msg.out_msg_val,msg);
	  result.out_msg.out_msg_len = strlen(msg);
	  return &result;
	}

	//	printf("Whether the file exists?\n");
	j = 0;
	for(i=0;i<NUM_FILE;i++){
	  if(j>=vdiskinfo.user[userindex].numoffile)break;
	  //	  printf("%d,%d\n",vdiskinfo.user[userindex].file[i].name[0]=='\0',i);
	  if(vdiskinfo.user[userindex].file[i].name[0]=='\0'){continue;}
	  j = j + 1;
	  if(!strcmp(vdiskinfo.user[userindex].file[i].name,file_name)){
	    
	    /*Delete information about this file and free memory*/
	   
	    /*Remove directory*/
	    vdiskinfo.user[userindex].file[i].name[0] = '\0';
	    vdiskinfo.user[userindex].numoffile--;

	    /*Free memory*/
	    for(k=0;k<vdiskinfo.user[userindex].file[i].numofblock;i++){
	      bunchindex =  vdiskinfo.user[userindex].file[i].position[k];
	      vdiskinfo.isoccupied[bunchindex] = 0;
	      pwrite(fildes,&content,sizeof(paragraph),bunchindex*BLOCKSIZE*8);
	      vdiskinfo.user[userindex].file[i].position[k] = 0;
	      vdiskinfo.availbunch++;
	    }

	    /*Save to system*/
	    pwrite(fildes,&vdiskinfo,sizeof(diskinfo),0);
	    close(fildes);

	    strcpy(msg,file_name);
	    strcat(msg," has been deleted!");
	    strcpy(result.out_msg.out_msg_val,msg);
	    result.out_msg.out_msg_len = strlen(msg);
	    close(fildes);
	    return &result;
	  }
	}
	
	strcpy(msg, file_name);
	strcat(msg," doesn't exist!");
	strcpy(result.out_msg.out_msg_val,msg);
	result.out_msg.out_msg_len = strlen(msg);
	close(fildes);
	return &result;	
}

write_output *
write_file_1_svc(write_input *argp, struct svc_req *rqstp)
{
	static write_output  result;
	
	FILE *vdisk; /*Virtual disk*/
	int fildes;/*file descriptor*/
	diskinfo vdiskinfo;
	off_t offset, numbytes;
	off_t diskoffset, remainbunchsize;
	paragraph content;

	char msg[100];

	char user_name[10], file_name[10];
	
	int i,j,k,offsetblock, userindex=0;
	int fileindex;

	/*Memory allocation for variables and initialization*/
	memset((write_output *)&result, 0,sizeof(write_output));
	result.out_msg.out_msg_val = malloc(100*sizeof(char));
	memset(result.out_msg.out_msg_val, 0,sizeof(char) * 100);

	memset((paragraph *)&content,0,sizeof(paragraph));

	/*Get current username and filename*/
	strcpy(user_name,argp->user_name);
	strcpy(file_name,argp->file_name);

	printf("Write function on server is invoked by %s!\n",user_name);

	if((vdisk=fopen("Virtual_Disk","r"))==NULL){
	  strcpy(msg, "Virtual Disk not constructed! ");
	  strcpy(result.out_msg.out_msg_val,msg);
	  result.out_msg.out_msg_len = strlen(msg);
	  //	  printf("%s\n",result.out_msg.out_msg_val);
	  return &result;
	}
	fclose(vdisk);
	
        fildes = open("Virtual_Disk",O_RDWR);
	pread(fildes,&vdiskinfo,sizeof(diskinfo),0);

	/*Check existence of user*/
	for(i=0;i<vdiskinfo.numofuser;i++){
	  if(!strcmp(vdiskinfo.user[i].name,user_name)){
	    userindex = i;
	    break;
	  }
	}
	
	if(userindex==-1){
	  strcpy(msg, "No files exist for ");
	  strcat(msg, user_name);
	  strcpy(result.out_msg.out_msg_val,msg);
	  result.out_msg.out_msg_len = strlen(msg);
	  close(fildes);
	  return &result;
	}

	//	printf("Whether the file exists?\n");
	j = 0;
	for(i=0;i<NUM_FILE;i++){
	  if(j>=vdiskinfo.user[userindex].numoffile)break;
	  //	  printf("%d,%d\n",vdiskinfo.user[userindex].file[i].name[0]=='\0',i);
	  if(vdiskinfo.user[userindex].file[i].name[0]=='\0'){continue;}
	  j = j + 1;
	  if(!strcmp(vdiskinfo.user[userindex].file[i].name,file_name)){
	    
	   
	    offset = argp->offset; numbytes = argp->numbytes;
	    if(offset > vdiskinfo.user[userindex].file[i].endoffile){
	      strcpy(msg, "offset exceed file");
	      strcpy(result.out_msg.out_msg_val,msg);
	      result.out_msg.out_msg_len = strlen(msg);
	      //	      printf("%s\n",result.out_msg.out_msg_val);
	      close(fildes);
	      return &result;
	    }
	    else{
	       /*Determine which block is offset in*/
	      offsetblock = offset/(BLOCKSIZE*8);//kth bunch of this file
	      //	      printf("%d-th bunch in disk\n",vdiskinfo.user[userindex].file[i].position[offsetblock]);
	      /*Find offset in virtual disk*/
	      diskoffset = offset - offsetblock * 8 * BLOCKSIZE + vdiskinfo.user[userindex].file[i].position[offsetblock] * 8 * BLOCKSIZE;
	      //	      printf("%d offset in disk\n",diskoffset);


	      /*determine whether additional block is needed*/
	      //	      printf("%d blocks used\n",vdiskinfo.user[userindex].file[i].numofblock);
	      //      printf("%d blocks needed\n",(offset+numbytes)/(8*BLOCKSIZE)+1);

	      if((offset+numbytes)/(8*BLOCKSIZE)+1>8){
		strcpy(msg, "FILE exceeds MAX SIZE. FAILED TO WRITE");
		strcpy(result.out_msg.out_msg_val,msg);
		result.out_msg.out_msg_len = strlen(msg);
		//		printf("%s\n",result.out_msg.out_msg_val);
		close(fildes);
		return &result;
	      }

	      /*Add additional blocks if needed*/
	      if((offset+numbytes)/(8*BLOCKSIZE)+1 >vdiskinfo.user[userindex].file[i].numofblock){
		/*Search occupied array to choose available bunch of blocks*/
		for(k=RES/8;k<NUM_BUNCH;k++){
		  if(vdiskinfo.isoccupied[k] == 0){
		    vdiskinfo.user[userindex].file[i].position[vdiskinfo.user[userindex].file[i].numofblock]=k;
		    vdiskinfo.user[userindex].file[i].numofblock++;
		    vdiskinfo.availbunch--;
		    if(vdiskinfo.user[userindex].file[i].numofblock>=((offset+numbytes)/(8*BLOCKSIZE)+1))break;
		  }
		}
	      }
	      
	      if(vdiskinfo.user[userindex].file[i].numofblock<((offset+numbytes)/(8*BLOCKSIZE)+1)){
		pwrite(fildes,&vdiskinfo,sizeof(diskinfo),0);
		strcpy(msg, "exceed memory. Failed to write");
		strcpy(result.out_msg.out_msg_val,msg);
		result.out_msg.out_msg_len = strlen(msg);
		//		printf("%s\n",result.out_msg.out_msg_val);
		close(fildes);
		return &result;
	      }
	      else{
		/*Write to file*/
		vdiskinfo.user[userindex].file[i].endoffile = (vdiskinfo.user[userindex].file[i].endoffile>(offset+numbytes)? vdiskinfo.user[userindex].file[i].endoffile:(offset+argp->buffer.buffer_len));
		//	printf("new end of file %d\n",vdiskinfo.user[userindex].file[i].endoffile);
		/*Start from offsetblock-th bunch write to file*/
		
		/*according to the lenght of the buffer*/
		/*First write to the bunch where offset is in*/
		remainbunchsize = (vdiskinfo.user[userindex].file[i].position[offsetblock]+1)* 8 * BLOCKSIZE-diskoffset;
		pwrite(fildes,argp->buffer.buffer_val,remainbunchsize,diskoffset);
		//	pread(fildes,&content,remainbunchsize,diskoffset);
		//	printf("%d,%s\n",diskoffset,content.words);

		/*Write to the other bunches*/
		for(k=offsetblock+1;k<vdiskinfo.user[userindex].file[i].numofblock;k++){
		  if(argp->buffer.buffer_len>remainbunchsize+(k-offsetblock-1)*8*BLOCKSIZE)
		    pwrite(fildes,argp->buffer.buffer_val+remainbunchsize+(k-offsetblock-1)*8*BLOCKSIZE,8*BLOCKSIZE,(vdiskinfo.user[userindex].file[i].position[k]*8*BLOCKSIZE));
		  pread(fildes,&content,8*BLOCKSIZE,(vdiskinfo.user[userindex].file[i].position[k]*8*BLOCKSIZE));
		  //	  printf("%s\n",content.words);
		}
			      
		pwrite(fildes,&vdiskinfo,sizeof(diskinfo),0);
		pread(fildes,&content,remainbunchsize,diskoffset);
		//	printf("%d,%s\n",diskoffset,content.words);
		strcpy(msg, "\nfile written successfully!");
		strcpy(result.out_msg.out_msg_val,msg);
		result.out_msg.out_msg_len = strlen(msg);
		//	printf("%s\n",result.out_msg.out_msg_val);
		close(fildes);
		return &result;
	      }
	    }	   
	  }
	}
	
	strcpy(msg, file_name);
	strcat(msg," doesn't exist!");
	strcpy(result.out_msg.out_msg_val,msg);
	result.out_msg.out_msg_len = strlen(msg);
	return &result;	
}

read_output *
read_file_1_svc(read_input *argp, struct svc_req *rqstp)
{
	static read_output  result;

		
	FILE *vdisk; /*Virtual disk*/
	int fildes;/*file descriptor*/
	diskinfo vdiskinfo;
	off_t offset, numbytes;
	off_t diskoffset, remainbunchsize,endread;
	paragraph content;

	char msg[10000];
	char intstr[10];

	char user_name[10], file_name[10];
	
	int i,j,k,l,offsetblock, userindex=0;
	int fileindex,numofread;

	offset = argp->offset; numbytes = argp->numbytes;

	/*Memory allocation for variables and initialization*/
	memset((read_output *)&result, 0,sizeof(read_output));
	result.out_msg.out_msg_val = malloc(10000*sizeof(char));
	memset(result.out_msg.out_msg_val, 0,sizeof(char) * 100);

	memset((paragraph *)&content,0,sizeof(paragraph));

	/*Get current username and filename*/
	strcpy(user_name,argp->user_name);
	strcpy(file_name,argp->file_name);

	printf("Read function on server is invoked by %s!\n",user_name);

	if((vdisk=fopen("Virtual_Disk","r"))==NULL){
	  strcpy(msg, "Virtual Disk not constructed! ");
	  strcpy(result.out_msg.out_msg_val,msg);
	  result.out_msg.out_msg_len = strlen(msg);
	  //	  printf("%s\n",result.out_msg.out_msg_val);
	  return &result;
	}
	fclose(vdisk);
	
        fildes = open("Virtual_Disk",O_RDONLY);
	pread(fildes,&vdiskinfo,sizeof(diskinfo),0);

	/*Check existence of user*/
	for(i=0;i<vdiskinfo.numofuser;i++){
	  if(!strcmp(vdiskinfo.user[i].name,user_name)){
	    userindex = i;
	    break;
	  }
	}
	
	if(userindex==-1){
	  strcpy(msg, user_name);
	  strcat(msg, " doesn't exit!");
	  strcpy(result.out_msg.out_msg_val,msg);
	  result.out_msg.out_msg_len = strlen(msg);
	  close(fildes);
	  return &result;
	}

	//	printf("Whether the file exists?\n");
	j = 0;
	for(i=0;i<NUM_FILE;i++){
	  if(j>=vdiskinfo.user[userindex].numoffile)break;
	  //	  printf("%d,%d\n",vdiskinfo.user[userindex].file[i].name[0]=='\0',i);
	  if(vdiskinfo.user[userindex].file[i].name[0]=='\0'){continue;}
	  j = j + 1;
	  if(!strcmp(vdiskinfo.user[userindex].file[i].name,file_name)){

	    if(offset > vdiskinfo.user[userindex].file[i].endoffile){
	      strcpy(msg, "offset exceed file");
	      strcpy(result.out_msg.out_msg_val,msg);
	      result.out_msg.out_msg_len = strlen(msg);
	      //	      printf("%s\n",result.out_msg.out_msg_val);
		close(fildes);
	      return &result;
	    }
	    else{
	      /*Determine which block is offset in*/
	      offsetblock = offset/(BLOCKSIZE*8);//kth bunch of this file
	      //	      printf("%d-th bunch in disk\n",vdiskinfo.user[userindex].file[i].position[offsetblock]);
	      /*Find offset in virtual disk*/
	      diskoffset = offset - offsetblock * 8 * BLOCKSIZE + vdiskinfo.user[userindex].file[i].position[offsetblock] * 8 * BLOCKSIZE;
	      //     printf("%d offset in disk\n",diskoffset);

	      endread = offset + numbytes;

	      /*Read file*/
	      /*	      if(vdiskinfo.user[userindex].file[i].endoffile==0){
		strcpy(msg, "file is empty");
		strcpy(result.out_msg.out_msg_val,msg);
		result.out_msg.out_msg_len = strlen(msg);
		printf("%s\n",result.out_msg.out_msg_val);
		endread = vdiskinfo.user[userindex].file[i].endoffile;
		close(fildes);
		return &result;
		}*/

	      /*Start from offsetblock-th bunch read file*/

	      
	      /*open file use fopen*/
	      vdisk = fopen("Virtual_Disk","r");
	      fseek(vdisk,diskoffset,SEEK_SET);
	     
	      strcpy(msg,"");
		
	      /*according to the numbytes*/
	      /*First read the bunch where offset is in*/
	      remainbunchsize = (vdiskinfo.user[userindex].file[i].position[offsetblock]+1)* 8 * BLOCKSIZE-diskoffset;
	      numofread = 0;/*The number of bytes which is already read*/
	      for(k=0;k<remainbunchsize;k++){
		if(numofread>=numbytes)break;
		if(numofread+offset>=vdiskinfo.user[userindex].file[i].endoffile){
		  strcat(msg,"\nEnd of file is reached. Only ");
		  snprintf(intstr,10,"%d",numofread);
		  strcat(msg,intstr);
		  strcat(msg, " bytes are read!");
		  break;
		}
		msg[numofread] = fgetc(vdisk);
		numofread++;
	      }
	      
	      /*read other bunches*/
	      for(k=offsetblock+1;k<=vdiskinfo.user[userindex].file[i].numofblock;k++){
		if(numofread>=numbytes)break;
		if(numofread+offset>=vdiskinfo.user[userindex].file[i].endoffile)break;
		fseek(vdisk,vdiskinfo.user[userindex].file[i].position[k]*8*BLOCKSIZE,SEEK_SET);
		for(l=0;l<8*BLOCKSIZE;l++){
		  if(numofread+offset>=vdiskinfo.user[userindex].file[i].endoffile){
		    strcat(msg,"\nEnd of file is reached. Only ");
		    snprintf(intstr,10,"%d",numofread);
		    strcat(msg,intstr);
		    strcat(msg, " bytes are read!");
		    break;
		  }
		  msg[numofread] = fgetc(vdisk);
		  numofread++;
		}		
	      }	      
	    }

	    strcat(msg,"\nRead Successfully!");
	    strcpy(result.out_msg.out_msg_val,msg);
	    result.out_msg.out_msg_len = strlen(result.out_msg.out_msg_val);
	    result.out_msg.out_msg_val[result.out_msg.out_msg_len] = '\0';
	    //	    printf("%s\n",result.out_msg.out_msg_val);
	    close(fildes);
	    fclose(vdisk);
	    return &result;
	    	   
	  }
	}
	strcpy(msg, file_name);
	strcat(msg," doesn't exist!");
	strcpy(result.out_msg.out_msg_val,msg);
	result.out_msg.out_msg_len = strlen(msg);
	close(fildes);
	return &result;	
}

copy_output *
copy_file_1_svc(copy_input *argp, struct svc_req *rqstp)
{
	static copy_output result;

	FILE *vdisk; /*Virtual disk*/
	int fildes;/*file descriptor*/
	diskinfo vdiskinfo;
	paragraph content;

	char msg[100];

	char user_name[10], from_filename[10], to_filename[10];
	
	int i,j,k, userindex=0;
	int from_fileindex=-1, to_fileindex=-1, fromnb,tonb,bunchindex;

	/*Memory allocation for variables and initialization*/
	memset((copy_output *)&result, 0,sizeof(copy_output));
	result.out_msg.out_msg_val = malloc(100*sizeof(char));
	memset(result.out_msg.out_msg_val, 0,sizeof(char) * 100);

	memset((paragraph *)&content,0,sizeof(paragraph));

	/*Get current username and filename*/
	strcpy(user_name,argp->user_name);
	strcpy(from_filename,argp->from_filename);
	strcpy(to_filename, argp->to_filename);

	printf("Copy function on server is invoked by %s!\n",user_name);

	if((vdisk=fopen("Virtual_Disk","r"))==NULL){
	  strcpy(msg, "Virtual Disk not constructed yet. No files exist for ");
	  strcat(msg, user_name);
	  strcpy(result.out_msg.out_msg_val,msg);
	  result.out_msg.out_msg_len = strlen(msg);
	  //	  printf("%s\n",result.out_msg.out_msg_val);
	  close(fildes);
	  return &result;
	}
	fclose(vdisk);
	
        fildes = open("Virtual_Disk",O_RDWR);
	pread(fildes,&vdiskinfo,sizeof(diskinfo),0);

	/*Check existence of user*/
	for(i=0;i<vdiskinfo.numofuser;i++){
	  if(!strcmp(vdiskinfo.user[i].name,user_name)){
	    userindex = i;
	    break;
	  }
	}
	
	if(userindex==-1){
	  strcpy(msg, "No files exist for ");
	  strcat(msg, user_name);
	  strcpy(result.out_msg.out_msg_val,msg);
	  result.out_msg.out_msg_len = strlen(msg);
	  close(fildes);
	  return &result;
	}

	//	printf("Whether the file exists?\n");
	j = 0;
	for(i=0;i<NUM_FILE;i++){
	  if(j>=vdiskinfo.user[userindex].numoffile)break;
	  //	  printf("%d,%d\n",vdiskinfo.user[userindex].file[i].name[0]=='\0',i);
	  if(vdiskinfo.user[userindex].file[i].name[0]=='\0'){printf("i=%d,j=%d\n",i,j);continue;}
	  j = j + 1;
	  if(!strcmp(vdiskinfo.user[userindex].file[i].name,from_filename)){
	    from_fileindex = i;
	  }
	  if(!strcmp(vdiskinfo.user[userindex].file[i].name, to_filename)){
	    to_fileindex = i;
	  }
	}
	
	if(from_fileindex == -1){
	  strcpy(msg, from_filename);
	  strcat(msg," doesn't exist!");
	  strcpy(result.out_msg.out_msg_val,msg);
	  result.out_msg.out_msg_len = strlen(msg);
	  close(fildes);
	  return &result;
	}

	if(to_fileindex == -1){
	  strcpy(msg, to_filename);
	  strcat(msg," doesn't exist!");
	  strcpy(result.out_msg.out_msg_val,msg);
	  result.out_msg.out_msg_len = strlen(msg);
	  close(fildes);
	  return &result;
	}

	/*Both file exist, copy from_file to to_file*/
	/*Modify to_file index info*/
	
	fromnb =  vdiskinfo.user[userindex].file[from_fileindex].numofblock;
	tonb = 	vdiskinfo.user[userindex].file[to_fileindex].numofblock;




	/*Assign blocks to to_file or free blocks*/
	if(tonb > fromnb){
	  for(k=fromnb+1;k<=tonb;k++){
	    bunchindex = vdiskinfo.user[userindex].file[to_fileindex].position[k];
	    vdiskinfo.isoccupied[bunchindex] = 0;
	    /*Initialize the bunch of blocks*/
	    pwrite(fildes,&content,sizeof(paragraph),bunchindex*BLOCKSIZE*8);
	    vdiskinfo.user[userindex].file[to_fileindex].position[k] = 0;
	  }
	}

	/*Assign new bunch to tofile*/
	if(tonb < fromnb){
	  for(k=RES/8;k<NUM_BUNCH;k++){
	    if(vdiskinfo.isoccupied[k] ==0){
	      vdiskinfo.user[userindex].file[to_fileindex].position[vdiskinfo.user[userindex].file[to_fileindex].numofblock] = k;
	      vdiskinfo.user[userindex].file[to_fileindex].numofblock++;
	      vdiskinfo.availbunch--;
	    }
	  }
	}

	if(vdiskinfo.user[userindex].file[to_fileindex].numofblock<fromnb){
	  pwrite(fildes,&vdiskinfo,sizeof(diskinfo),0);
	  strcpy(msg, "exceed memory. Failed to copy");
	  strcpy(result.out_msg.out_msg_val,msg);
	  result.out_msg.out_msg_len = strlen(msg);
	  //	  printf("%s\n",result.out_msg.out_msg_val);
	  close(fildes);
	  return &result;
	}
	       
	vdiskinfo.user[userindex].file[to_fileindex].endoffile = vdiskinfo.user[userindex].file[from_fileindex].endoffile;

	

	for(k=0;k<fromnb;k++){
	  /*readfirst*/
	  bunchindex = vdiskinfo.user[userindex].file[from_fileindex].position[k];
	  pread(fildes,&content,sizeof(paragraph),bunchindex*8*BLOCKSIZE);
	  /*then write*/
	  bunchindex =  vdiskinfo.user[userindex].file[to_fileindex].position[k];
	  pwrite(fildes,&content,sizeof(paragraph),bunchindex*8*BLOCKSIZE);
	}

	pwrite(fildes,&vdiskinfo,sizeof(diskinfo),0);
	
	strcpy(msg, "Copy successfully");
	strcpy(result.out_msg.out_msg_val,msg);
	result.out_msg.out_msg_len = strlen(msg);
	//	printf("%s\n",result.out_msg.out_msg_val);
	close(fildes);
	return &result;
	

}

/* shared memory for students record */
#define KEYSR ((key_t)(111764))//key id of shared memory 1+last five digits of my SID
#define SEGSIZESR sizeof(struct StudentInfo)//size of shared memory
/* sharede memory for read_count*/
#define KEYRC ((key_t)(211764))
#define SEGSIZERC sizeof(int)
/* shared memory for number of students */
#define KEYNS ((key_t)(311764))
#define SEGSIZENS sizeof(unsigned int)


#define NUM_SEMAPHS 2
#define SEMA_KEY ((key_t)(11764)) //key id of semaphores: last five digits of my SID

struct StudentInfo{
  char Name[40];
  char SID[15];
  char Address[100];
  char TelNum[15];
};

  void Wait(int semaph, int n);
  void Signal(int semaph, int n);
  int GetSemaphs(key_t, int n);

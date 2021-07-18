#include <stdio.h>
#include <sys/shm.h>  //shmget,shmat,shmdt 相关声明都在这里
#include <sys/sem.h>  //sembuf, SEM_UNDO,semget,semop 相关声明和宏定义都在这里
#include <string.h>

#define SHMSIZE 1024

typedef struct sembuf SB;  //将sembuf重命名

int main()
{
  int res=-1,shmid=0,semid=0;
  key_t  key=IPC_PRIVATE;
  char *shmaddr=NULL;
  SB sem_v0={0,1,SEM_UNDO},  //构建对第一个信号量进行V操作的参数
     sem_p1={1,-1,SEM_UNDO}; //构建对第二个信号量进行P操作的参数

  if(-1 == (key=ftok("/",888))) //生成key
  {
    perror("ftok");
    return res;
  }
  if (0 > (shmid=shmget(key,SHMSIZE,IPC_CREAT|0600))) //使用key创建或获取共享内存ID,大小为1024字节 or use getpagesize() instead
  {
    perror("shmget");
    return res;
  }
  else printf("created shared memory :%d\n",shmid); //将共享内存的ID进行显示
  
  if ((char *)0 > (shmaddr=shmat(shmid,0,0))) //通过共享内存的ID获取内存地址
  {
    perror("shmat");
    return res;
  }
  else printf("attached shared memory:%p\n",shmaddr); //将共享内存的地址进行打印
  
  if (0 > (semid=semget(key,2,IPC_CREAT|0600))) //通过key创建两个信号量的ID
  {
    perror("semget");
    return res;
  }
  else printf("created a sem set with two sems which id is :%d\n",semid);

  do
  {
    if (0 > semop(semid,&sem_p1,1))  //对第二个信号量进行P操作
    {
      perror("semop"); 
      return res;
    }
    printf("from shm :%s",shmaddr); //将共享内存中的内容进行打印
    
    if(0 == strcmp(shmaddr,"quit\n"))break; //如果内容为 quit，则直接退出循环
    if (0 > semop(semid,&sem_v0,1)) //对第一个信号量进行V操作
    {
      perror("semop"); 
      return res;
    }
  }while(1);

  if (0 > (shmdt(shmaddr))) //分享共享内存
  {
    perror("shmdt");
    return res;
  }
  else   printf("Deattach shared-memory\n");
  
  if (0 > semop(semid,&sem_v0,1)) //对第一个信号量进行V操作
  {
    perror("semop"); 
    return res;
  }
  
  res=0;
  return res;
}
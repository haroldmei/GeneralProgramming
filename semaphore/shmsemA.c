#include <stdio.h>
#include <sys/shm.h> //shmget,shmat,shmdt,shmctl 相关声明都在这里
#include <sys/sem.h> //sembuf,SEM_UNDO,SETALL 相关声明和宏定义都在这里
#include <string.h>  //memset,strcmp  相关函数声明都在这里

#define SHMSIZE 1024

typedef struct sembuf SB; // 将sembuf重命名

union semun  //定义semun共用体作为参数 
{ 
  int val;		 // value for SETVAL  设定一个值可以用
  struct semid_ds *buf;	 // buffer for IPC_STAT & IPC_SET  获取状态可以使用
  unsigned short *array; // array for GETALL & SETALL  设定所有值或获取所有值可以使用
  struct seminfo *__buf; // buffer for IPC_INFO 获取信息可以用
  void *__pad; //万能指针
};

int main() 
{
  int res=-1,shmid=0,semid=0;
  key_t  key=IPC_PRIVATE;
  char *shmaddr=NULL; //定义一个共享内存的指针
  SB sem_p0={0,-1,SEM_UNDO}, //构建对第一个信号量进行P操作的参数
     sem_v0={0,1,SEM_UNDO}, //构建对第一个信号量进行V操作的参数
     sem_v1={1,1,SEM_UNDO}; //构建对第二个信号量进行V操作的参数
  union semun sem_args;  
  unsigned short array[2]={0,0}; //构建数组

  sem_args.array=array; //构建出给两个信号量一起赋值的共用体参数
  

  if(-1 == (key=ftok("/",888))) //生成key
  {
    perror("ftok");
    return res;
  }
  if (0 > (shmid=shmget(key,SHMSIZE,IPC_CREAT|0600))) //使用shmget 创建或获取共享内存ID , 大小为1024个字节 ， or use getpagesize() instead 
  {
    perror("shmget");
    return res;
  }
  else printf("created shared memory :%d\n",shmid); //显示出获取的共享内存ID
  
  if ((char *)0 > (shmaddr=shmat(shmid,0,0))) //通过ID获取地址，并且使用shmaddr进行指向
  {
    perror("shmat");
    return res;
  }
  else printf("attached shared memory:%p\n",shmaddr); //将内存地址打印出来
  
  if (0 > (semid=semget(key,2,IPC_CREAT|0600))) //通过key获取两个信号量的ID
  {
    perror("semget");
    return res;
  }
  else printf("created a sem set with two sems which id is :%d\n",semid); //将信号量ID打印出来

  if (0 > semctl(semid,0,SETALL,sem_args)) //将两个信号量一起赋值为0，设置值存于sem_args中
  {
    perror("semctl");
    return res;
  } 
  else printf("semset has been initialized\n");
  
  if (0 > semop(semid,&sem_v0,1)) //对第一个信号量进行V操作
  {
    perror("semop"); 
    return res;
  }

  memset(shmaddr,0,SHMSIZE); //将共享内存置0

  do
  {
    if (0 > semop(semid,&sem_p0,1)) //对第一个信号量进行P操作
    {
      perror("semop"); 
      return res;
    }
    puts("please enter the message to shm:\n('quit' to exit)");
    if(NULL == (fgets(shmaddr,SHMSIZE,stdin))) //将输入内容写到共享内存中
    {
      perror("fgets");
      return res;
    }
    if (0 > semop(semid,&sem_v1,1)) //对第二个信号量进行V操作
    {
      perror("semop"); 
      return res;
    }
  }while(strcmp(shmaddr,"quit\n"));   //如果输入为quit就退出

  if (0 > (shmdt(shmaddr))) //分离共享内存
  {
    perror("shmdt");
    return res;
  }
  else   printf("Deattach shared-memory\n"); 
  
  if (0 > semop(semid,&sem_p0,1)) //对第一个信号量进行P操作
  {
    perror("semop"); 
    return res;
  }

  if (0 > shmctl(shmid, IPC_RMID, NULL)) //删除和释放共享内存
  {
    perror("shmctl(IPC_RMID)\n");
    return res;
  }
  else printf("Delete shared-memory\n"); 
  res=0;
  return res;
}
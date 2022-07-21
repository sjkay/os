#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESS_NUM 100

#define ARRIVAL 0
#define BURSTTIMEREMAIN 1
#define PID 2

#define FCFS 0
#define SJF 1
#define RR 2

//Process structure
typedef struct process* proPointer;
typedef struct process{
  int pid;
  int bursttime;
  int arrival;
  int bursttime_remain;
  int timequantum;
  int waitingTime;

}process;

//Process information
proPointer GjobQ[MAX_PROCESS_NUM];
int GjQ_front, GjQ_rear;

//ready queue 
proPointer readyQ[MAX_PROCESS_NUM];
int rQ_front, rQ_rear;

//waiting queue 
proPointer waitQ[MAX_PROCESS_NUM];
int wQ_front, wQ_rear;

proPointer termQ[MAX_PROCESS_NUM];
int tQ_front, tQ_rear;

//job queue
proPointer jobQ[MAX_PROCESS_NUM];
int jQ_front, jQ_rear;

double wT[3] = {0};

void init_GjobQ(){
  GjQ_front = -1;
  GjQ_rear = -1;

  for(int i = 0; i < MAX_PROCESS_NUM; i++){
    GjobQ[i] = NULL;
  }
}

void add_GjobQ(proPointer newP){
  if(GjQ_rear == MAX_PROCESS_NUM - 1)
    printf("GjobQ is FULL");
  else
    GjobQ[++GjQ_rear] = newP;
}

proPointer poll_GjobQ(){
  if(GjQ_front == GjQ_rear)
    printf("GjobQ is EMPTY");
  else
    return GjobQ[++GjQ_front];
}


//ready queue
void init_readyQ(){
  rQ_front = -1;
  rQ_rear = -1;

  for(int i = 0; i < MAX_PROCESS_NUM; i++){
    readyQ[i] = NULL;
  }
}

void add_readyQ(proPointer newP){
  if(rQ_rear == MAX_PROCESS_NUM - 1)
    printf("readyQ is FULL");
  else
    readyQ[++rQ_rear] = newP;
}

proPointer poll_readyQ(){
  if(rQ_front == rQ_rear)
    printf("readyQ is EMPTY");
  else
    return readyQ[++rQ_front];
}

//wait queue
void init_waitQ(){
  wQ_front = -1;
  wQ_rear = -1;

  for(int i = 0; i < MAX_PROCESS_NUM; i++){
    waitQ[i] = NULL;
  }
}

void add_waitQ(proPointer newP){
  if(wQ_rear == MAX_PROCESS_NUM - 1)
    printf("waitQ is FULL");
  else
    waitQ[++wQ_rear] = newP;
}

//termination queue
void init_termQ(){
  tQ_front = -1;
  tQ_rear = -1;

  for(int i = 0; i < MAX_PROCESS_NUM; i++){
    termQ[i] = NULL;
  }
}

void add_termQ(proPointer newP){
  if(tQ_rear == MAX_PROCESS_NUM - 1)
    printf("termQ is FULL");
  else
    termQ[++tQ_rear] = newP;
}

proPointer poll_termQ(){
  if(tQ_front == tQ_rear)
    printf("termQ is EMPTY");
  else
    return termQ[++tQ_front];
}
void printQ_Gjob(){
  for(int i = 0; i < (GjQ_rear - GjQ_front); i++){
    printf("p%d , ", GjobQ[i]->pid);
    printf("burst time %d, ", GjobQ[i]->bursttime);
    printf("arrival %d, \n", GjobQ[i]->arrival);
  }
}


//job queue
void init_jobQ(){
  jQ_front = -1;
  jQ_rear = -1;

  for(int i = 0; i < MAX_PROCESS_NUM; i++){
    jobQ[i] = NULL;
  }
}

void add_jobQ(proPointer newP){
  if(jQ_rear == MAX_PROCESS_NUM - 1)
    printf("jobQ is FULL");
  else
    jobQ[++jQ_rear] = newP;
}

proPointer poll_jobQ(){
  if(jQ_front == jQ_rear)
    printf("jobQ is EMPTY");
  else
    return jobQ[++jQ_front];
}


//background ready queue
void clone_jobQ(){
  for(int i = GjQ_front+1; i <= GjQ_rear; i++){
    proPointer newP = (proPointer)malloc(sizeof(struct process));
    newP->pid = GjobQ[i]->pid;
    newP->bursttime = GjobQ[i]->bursttime;
    newP->arrival = GjobQ[i]->arrival;
    newP->bursttime_remain = GjobQ[i]->bursttime_remain;
    newP->timequantum = GjobQ[i]->timequantum;
    newP->waitingTime = GjobQ[i]->waitingTime;

    add_jobQ(newP);
  }
}


//arrival time: 0, bursttime_remain: 1,  pid: 2
void merge(proPointer list[], int p, int q, int r, int type){
  int n1 = q - p + 1;
  int n2 = r - q ;
  proPointer L[n1 + 1];
  proPointer R[n1 + 1];
  int i, j;
  for(i = 0; i < n1; i++){
    L[i] = list[p + i];
  }
  proPointer dummy1 = (proPointer)malloc(sizeof(struct process));
  switch (type) {
    case ARRIVAL:
      dummy1 -> arrival = 1000000;
      break;
    case BURSTTIMEREMAIN:
      dummy1 -> bursttime_remain = 1000000;
      dummy1 -> arrival = 1000000;
      dummy1 -> pid = 1000000;
      break;
    case PID:
      dummy1 -> pid = 1000000;
      break;
    default:
      printf("merge: unknown type received\n");
      break;
  }
  L[n1] = dummy1;
  for(j = 0; j < n2; j++){
    R[j] = list[q + 1 + j];
  }
  proPointer dummy2 = (proPointer)malloc(sizeof(struct process));
  switch (type) {
    case ARRIVAL:
      dummy2 -> arrival = 1000000;
      break;
    case BURSTTIMEREMAIN:
      dummy2 -> bursttime_remain = 1000000;
      dummy2 -> arrival = 1000000;
      dummy2 -> pid = 1000000;
      break;
    case PID:
      dummy2 -> pid = 1000000;
      break;
    default:
      printf("merge: unknown type received\n");
      break;
  }
  R[n2] = dummy2;

  i = 0; j = 0;
  for(int k = p; k <= r; k++){
    switch(type){
      case ARRIVAL:
      if(L[i]->arrival <= R[j]->arrival){
        list[k] = L[i];
        i++;
      }
      else{
        list[k] = R[j];
        j++;
      }
      break;

      case BURSTTIMEREMAIN:
      if(L[i]->bursttime_remain < R[j]->bursttime_remain){
        list[k] = L[i];
        i++;
      }
      else if(L[i]->bursttime_remain == R[j]->bursttime_remain && L[i]->arrival < R[j]->arrival){
        list[k] = L[i];
        i++;
      }
      else if(L[i]->bursttime_remain == R[j]->bursttime_remain && L[i]->arrival == R[j]->arrival && L[i]->pid < R[j]->pid){
        list[k] = L[i];
        i++;
      }
      else{
        list[k] = R[j];
        j++;
      }
      break;

      case PID:
      if(L[i]->pid <= R[j]->pid){
        list[k] = L[i];
        i++;
      }
      else{
        list[k] = R[j];
        j++;
      }
      break;

      default:
      printf("unknown type\n");
      break;
    }
  }
  free(dummy1);
  free(dummy2);
}

void mergesort(proPointer list[], int p, int r, int type){
  switch(type){
    case ARRIVAL:
    if(p < r){
      int q = (p+r)/2;
      mergesort(list, p, q, ARRIVAL);
      mergesort(list, q+1, r, ARRIVAL);
      merge(list, p, q, r, ARRIVAL);
    }
    break;

    case BURSTTIMEREMAIN:
    if(p < r){
      int q = (p+r)/2;
      mergesort(list, p, q, BURSTTIMEREMAIN);
      mergesort(list, q+1, r, BURSTTIMEREMAIN);
      merge(list, p, q, r, BURSTTIMEREMAIN);
    }
    break;


    case PID:
    if(p < r){
      int q = (p+r)/2;
      mergesort(list, p, q, PID);
      mergesort(list, q+1, r, PID);
      merge(list, p, q, r, PID);
    }
    break;

    default:
    break;
  }
}


//create_processes, input: num_process

void create_processes(int num_process, int tq){

  srand( (unsigned)time(NULL) );

  init_GjobQ();

  for(int i = 0; i < num_process; i++){
    proPointer newP = (proPointer)malloc(sizeof(struct process));
    newP->pid = i+1;
    newP->bursttime = rand() % 25 + 2; //CPU burst time 2 ~ 26
    newP->arrival = rand() % (num_process + 10);
    newP->bursttime_remain = newP -> bursttime;
    newP->timequantum = tq;
    newP->waitingTime = 0;

    add_GjobQ(newP);
  }
  printQ_Gjob();

  }

  void wait(int pid){
    for(int i = rQ_front+1; i <= rQ_rear; i++){
      if(readyQ[i]->pid != pid){
        readyQ[i]->waitingTime++;
      }
    }
  }

  int isEmpty(int front, int rear){
    if(front == rear)
      return 1; //true;
    else
      return 0;//false;
  }

  proPointer poll_waitQ(int pid){
    proPointer res;
    if(wQ_front == wQ_rear)
      printf("waitQ is EMPTY");
    else{
      for(int i = wQ_front+1; i <= wQ_rear; i++){
        if(waitQ[i]->pid == pid && i == wQ_rear){
          wQ_rear--;
          return waitQ[i];
        }
        else if(waitQ[i]->pid == pid){
          res = waitQ[i];
          for(int j = i+1; j <= wQ_rear; j++){
            waitQ[j-1] = waitQ[j];
          }
          wQ_rear--;
          return res;
        }
      }
    }
  }


  void evaluation(int type){
    mergesort(termQ, tQ_front+1, tQ_rear, 4);
    int num = tQ_rear - tQ_front;
    int sum_waittime = 0;
    double avg_waittime = 0;
    for(int i = tQ_front+1; i <= tQ_rear ; i++){
      printf("pid: %d, waiting time: %d\n",
        termQ[i]->pid, termQ[i]->waitingTime);
        sum_waittime += termQ[i]->waitingTime;
    }
    avg_waittime = (double)sum_waittime/num;
    printf("average waiting time: %f\n", avg_waittime);
    wT[type] = avg_waittime;
  }


void FCFS_alg(int num_process){
  printf("\n********************FCFS algorithm********************\n");
  srand( (unsigned)time(NULL) );

  init_jobQ();
  clone_jobQ();

  mergesort(jobQ, jQ_front+1, jQ_rear, ARRIVAL);
  init_readyQ();
  init_waitQ();
  init_termQ();

  int nowTime = 0;
  int idle = 0;
  int check = 0;
  proPointer runP = NULL;
  for(nowTime = 0; check < num_process; nowTime++){

    if(isEmpty(jQ_front, jQ_rear) != 1){
      for(int i = jQ_front+1; i <= jQ_rear; i++){
        if(jobQ[i]->arrival == nowTime)
          add_readyQ(poll_jobQ());
      }
    }

    if(isEmpty(rQ_front, rQ_rear)!=1 && runP == NULL){
      runP = poll_readyQ();
    }

    if(runP==NULL && isEmpty(wQ_front, wQ_rear)){
      printf("-- ");
      idle++;
    }

    else if(runP==NULL && isEmpty(wQ_front, wQ_rear)!=1){
      printf("-- ");
      idle++;
    }

    else if(runP != NULL){
      printf("p%d ", runP->pid);
      runP->bursttime_remain--;
      wait(runP->pid);

      if(runP->bursttime_remain == 0){
        add_termQ(runP);
        check++;
        runP = NULL;
      }
    
    }
  }
  printf("\n");
  evaluation(FCFS);

}


void SJF_alg(int num_process){
  printf("\n********************Nonpreemptive SJF algorithm********************\n");
  srand( (unsigned)time(NULL) );

  init_jobQ();
  clone_jobQ();

  mergesort(jobQ, jQ_front+1, jQ_rear, ARRIVAL);
  init_readyQ();
  init_waitQ();
  init_termQ();

  int nowTime = 0;
  int idle = 0;
  int check = 0;
  proPointer runP = NULL;

  for(nowTime = 0; check < num_process; nowTime++){

    if(isEmpty(jQ_front, jQ_rear) != 1){
      for(int i = jQ_front+1; i <= jQ_rear; i++){
        if(jobQ[i]->arrival == nowTime)
          add_readyQ(poll_jobQ());
      }
    }

    if(isEmpty(rQ_front, rQ_rear)!=1 && runP == NULL){
      mergesort(readyQ, rQ_front+1, rQ_rear, BURSTTIMEREMAIN);
      runP = poll_readyQ();
    }

    if(runP==NULL && isEmpty(wQ_front, wQ_rear)){
      printf("-- ");
      idle++;
    }
    else if(runP==NULL && isEmpty(wQ_front, wQ_rear)!=1){
      printf("-- ");
      idle++;
    }
    else if(runP != NULL){
      printf("p%d ", runP->pid);
      runP->bursttime_remain--;
      wait(runP->pid);

      if(runP->bursttime_remain == 0){
        add_termQ(runP);
        check++;
        runP = NULL;
      }
    }
  }
  printf("\n");
  evaluation(SJF);
}

void RR_alg(int num_process, int tq){
  printf("\n********************ROUND ROBIN algorithm********************\n");
  srand( (unsigned)time(NULL) );

  init_jobQ();
  clone_jobQ();

  mergesort(jobQ, jQ_front+1, jQ_rear, ARRIVAL);
  init_readyQ();
  init_waitQ();
  init_termQ();

  int nowTime = 0;
  int idle = 0;
  int check = 0;
  proPointer runP = NULL;

  for(nowTime = 0; check < num_process; nowTime++){
    if(isEmpty(jQ_front, jQ_rear) != 1){
      for(int i = jQ_front+1; i <= jQ_rear; i++){
        if(jobQ[i]->arrival == nowTime)
        add_readyQ(poll_jobQ());
      }
    }

    if(isEmpty(rQ_front, rQ_rear)!=1 && runP == NULL){
      runP = poll_readyQ();
    }

    if(runP==NULL && isEmpty(wQ_front, wQ_rear)){
      printf("-- ");
      idle++;
    }
    else if(runP==NULL && isEmpty(wQ_front, wQ_rear)!=1){
      printf("-- ");
      idle++;
    }
    else if(runP != NULL){
      printf("p%d ", runP->pid);
      runP->bursttime_remain--;
      runP->timequantum--;
      wait(runP->pid);



      if(runP->bursttime_remain == 0){
        add_termQ(runP);
        check++;
        runP = NULL;
      }

      if(runP != NULL && runP->timequantum == 0){
        runP->timequantum = tq;
        runP->arrival = nowTime;
        add_readyQ(runP);
        runP = poll_readyQ();
      }

    }
  }
  printf("\n");
  evaluation(RR);
}


int main(int argc, char **argv){
  int num_process, tq;

  printf("Starting FCFS, Nonpreemptive SJF and RR algorithm Evaluation\n");
  printf("Number of processes: ");
  scanf("%d", &num_process);
  printf("Time quantum: ");
  scanf("%d", &tq);

  create_processes(num_process, tq);
  FCFS_alg(num_process);
  SJF_alg(num_process);
  RR_alg(num_process, tq);

  return 0;
}

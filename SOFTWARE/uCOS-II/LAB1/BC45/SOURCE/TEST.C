#include "includes.h"

// CONSTANTS
#define TASK_STK_SIZE 512 // Size of each task's stacks (# of WORDs)
#define N_TASKS 3 // Number of identical tasks

// VARIABLES
OS_STK TaskStk[N_TASKS][TASK_STK_SIZE]; // Tasks stacks
OS_STK TaskStartStk[TASK_STK_SIZE];
char   TaskData[N_TASKS]; // Parameters to pass to each task
//OS_EVENT *RandomSem;
OS_EVENT *R1;
OS_EVENT *R2;
// FUNCTION PROTOTYPES
// void BaseTask(int _taskId, int _computeTime, int _period, int _isPrint);
void Task1(void *pdata); // Function prototypes of Startup task
void Task2();  
void Task3();  

void PrintMsgList();
void InitMsgList();
void Preeptwait(int tick);
static  void  AddMsgList(int _tick, int _event, int _fromTaskId, int _toTaskId) ;


// MAIN
void main(void) {
  INT8U err;
  INT8U err1;
  // Initialize uC/OS-II
  OSInit();
  // Save environment to return to DOS
  PC_DOSSaveReturn();
  // Install uC/OS-II's context switch vector
  PC_VectSet(uCOS, OSCtxSw);
  
  // Initialize message list
  InitMsgList();
  // Create tasks

  R1 = OSMutexCreate(1, &err);
  if (err != OS_NO_ERR) {
    AddMsgList(OSTimeGet(), 0, 888, 1);
  }
  R2 = OSMutexCreate(2, &err);
  if (err != OS_NO_ERR) {
    AddMsgList(OSTimeGet(), 0, 777, 1);
  }
  OSTaskCreate(Task1, (void *)0, &TaskStk[0][TASK_STK_SIZE-1], 3);
  OSTaskCreate(Task2, (void *)0, &TaskStk[1][TASK_STK_SIZE-1], 4);
  OSTaskCreate(Task3, (void *)0, &TaskStk[2][TASK_STK_SIZE-1], 5);

  // Start multitasking
  OSStart();
}
/*
void BaseTask(int _taskId, int _computeTime, int _period, int _isPrint) {
  INT16S key;
  int start, end, toDelay, deadline;
  OSTCBCur->computeTime = _computeTime;
  OSTCBCur->period = _period;
  //deadline = _period;
    
  while (1) {
    // See if key has been pressed
    if (PC_GetKey(&key) == TRUE) {                     
      // Yes, see if it's the ESCAPE key
      if (key == 0x1B) {
        // Return to DOS
        PC_DOSReturn();
      }
    }

    // 取得開始時間
    start = OSTimeGet();
    // 等待task執行結束
    while (OSTCBCur->computeTime > 0)
      ; // Busywaiting
    // 取得結束時間
    end = OSTimeGet();
    // 計算完成時間與期望時間的差 -> 期望花的時間:period, 實際花的時間:end-start 
    toDelay = OSTCBCur->period - (end - start);
    // 計算下一輪開始時間
    start += OSTCBCur->period;
    // 重製執行時間
    OSTCBCur->computeTime = _computeTime;


    if (_isPrint) {
      OS_ENTER_CRITICAL();
      PrintMsgList();
      OS_EXIT_CRITICAL();
    }
    OSTimeDly(toDelay);
    // 檢查此task是否超時

    
    if (toDelay < 0) { // 超時
      OS_ENTER_CRITICAL();
      printf("%d\tTask%d Deadline!\n", deadline, _taskId);
      OS_EXIT_CRITICAL();
    }
    else { // 未超時
      if (_isPrint) {
        OS_ENTER_CRITICAL();
        PrintMsgList();
        OS_EXIT_CRITICAL();
      }
      OSTimeDly(toDelay);
    }
    

    // 將deadline增加至下一周期
    // deadline += _period;
  }
}
*/
// Task1 (STARTUP TASK)
void Task1(void *pdata) {
  INT16S key;
  INT8U err;
  int start, end, toDelay, deadline;
  // Allocate storage for CPU status register
#if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr;
#endif
  // Prevent compiler warning
  pdata = pdata;

  OS_ENTER_CRITICAL();
  // Install uC/OS-II's clock tick ISR
  PC_VectSet(0x08, OSTickISR);
  // Reprogram tick rate
  PC_SetTickRate(OS_TICKS_PER_SEC);
  OS_EXIT_CRITICAL();

  // Initialize uC/OS-II's statistics
  // OSStatInit函數似乎會造成tick計算異常，因此選擇註解掉
  // OSStatInit();
  
  // 在做完start up task所需額外做的事之後，將tick歸零  
  OSTimeSet(0);

  // set 1
   OSTimeDly(8);
   OSTCBCur->computeTime = 6;

  // set 2
  //OSTimeDly(5);
  //OSTCBCur->computeTime = 11;
  OSTCBCur->period = 60;

  while (1) {
    // See if key has been pressed
    if (PC_GetKey(&key) == TRUE) {                     
      // Yes, see if it's the ESCAPE key
      if (key == 0x1B) {
        // Return to DOS
        PC_DOSReturn();
      }
    }
    start = OSTimeGet();

  
    // set1
    while (OSTCBCur->computeTime > 4){}
    OSMutexPend(R1, 0, &err);
    while (OSTCBCur->computeTime > 2){}
    OSMutexPend(R2, 0, &err);
    while (OSTCBCur->computeTime > 0){}
    OSMutexPost(R2);
    OSMutexPost(R1);
    

    // set 2
    /*
    while (OSTCBCur->computeTime > 9){}
    OSMutexPend(R2, 0, &err);
    while (OSTCBCur->computeTime > 6){}
    OSMutexPend(R1, 0, &err);
    while (OSTCBCur->computeTime > 3){}
    OSMutexPost(R1);
    while (OSTCBCur->computeTime > 0){}
    OSMutexPost(R2);
    */

    end = OSTimeGet();
    toDelay = OSTCBCur->period - (end - start);
    start += OSTCBCur->period;

    //set1
    OSTCBCur->computeTime = 6;

    //set2
    // OSTCBCur->computeTime = 11;

    OS_ENTER_CRITICAL();
    PrintMsgList();
    OS_EXIT_CRITICAL();
    OSTimeDly(toDelay);
  }
}

// Task2
void Task2() {
  INT16S key;
  INT8U err;
  int start, end, toDelay, deadline;
  // set 1
  OSTimeDly(4);
  OSTCBCur->computeTime = 6;

  // set 2
  // OSTCBCur->computeTime = 12;
  OSTCBCur->period = 60;
  while (1) {
    // See if key has been pressed
    if (PC_GetKey(&key) == TRUE) {                     
      // Yes, see if it's the ESCAPE key
      if (key == 0x1B) {
        // Return to DOS
        PC_DOSReturn();
      }
    }
    start = OSTimeGet();

    // set1
    while (OSTCBCur->computeTime > 4){}
    OSMutexPend(R2, 0, &err);
    while (OSTCBCur->computeTime > 0){}
    OSMutexPost(R2);
    
    // set2
    /*
    while (OSTCBCur->computeTime > 10){}
    OSMutexPend(R1, 0, &err);
    while (OSTCBCur->computeTime > 4){}
    OSMutexPend(R2, 0, &err);
    while (OSTCBCur->computeTime > 2){}
    OSMutexPost(R2);
    while (OSTCBCur->computeTime > 0){}
    OSMutexPost(R1);  
    */
    
    end = OSTimeGet();
    toDelay = OSTCBCur->period - (end - start);
    start += OSTCBCur->period;

    // set1
    OSTCBCur->computeTime = 6;
    // set2
    //OSTCBCur->computeTime = 12;

    OSTimeDly(toDelay);
  }
}

// Task3
void Task3() {
  INT16S key;
  INT8U err;
  int start, end, toDelay, deadline;
  OSTCBCur->computeTime = 9;
  OSTCBCur->period = 60;
  //deadline = _period;
  while (1) {
    // See if key has been pressed
    if (PC_GetKey(&key) == TRUE) {                     
      // Yes, see if it's the ESCAPE key
      if (key == 0x1B) {
        // Return to DOS
        PC_DOSReturn();
      }
    }

    start = OSTimeGet();

    while (OSTCBCur->computeTime > 7){}
    OSMutexPend(R1, 0, &err);
    while (OSTCBCur->computeTime > 0){}
    OSMutexPost(R1);
    
    end = OSTimeGet();
    toDelay = OSTCBCur->period - (end - start);
    start += OSTCBCur->period;
    OSTCBCur->computeTime = 9;
    OSTimeDly(toDelay);
  }
}

void PrintMsgList() {
  char s[20];
  while (msgList->next) {
    // 印出訊息佇列節點訊息
    if (msgList->next->event == 0)
    {
      strcpy(&s[0], "Preempt  ");
    }
    else if (msgList->next->event == 1)
    {
      strcpy(&s[0], "Complete  ");
    }
    else if (msgList->next->event == 2) 
    {
      strcpy(&s[0], "Lock R1   ");
    }
    else if (msgList->next->event == 3) 
    {
      strcpy(&s[0], "Lock R2   ");
    }
    else if (msgList->next->event == 4) 
    {
      strcpy(&s[0], "UnLock R1 ");
    }
    else if (msgList->next->event == 5) 
    {
      strcpy(&s[0], "UnLock R2 ");
    }
    printf("%d\t%s\t%d\t%d\n", 
      msgList->next->tick,
      s,
      msgList->next->fromTaskId,
      msgList->next->toTaskId
    );
    // 將印過的節點刪掉
    msgTemp = msgList;
    msgList = msgList->next;
    free(msgTemp);
  }
}

void InitMsgList() {
  // 新增dummy節點(簡化串列操作)
  msgList = (msg*)malloc(sizeof(msg));
  msgList->next = (msg*)0;
}

void Preeptwait(int tick) {
#if OS_CRITICAL_METHOD==3
    OS_CPU_SR cpu_sr = 0;
#endif
    int now, exit;
    OS_ENTER_CRITICAL();
    now = OSTimeGet();
    exit = now + tick;
    //printf("exit: %d\n", exit);
    OS_EXIT_CRITICAL();
    while (1) {
        if (OSTimeGet() == exit) {
            break;
        }
    }
}


static  void  AddMsgList(int _tick, int _event, int _fromTaskId, int _toTaskId) {
    /* 尋找訊息佇列尾端 */
    msgTemp = msgList;
    while (msgTemp->next)
        msgTemp = msgTemp->next;
    /* 增加一個節點到訊息佇列 */
    msgTemp->next = (msg*)malloc(sizeof(msg));
    msgTemp->next->tick = _tick;
    msgTemp->next->event = _event;
    msgTemp->next->fromTaskId = _fromTaskId;
    msgTemp->next->toTaskId = _toTaskId;
    msgTemp->next->next = (msg*)0;
}
/**
 * @file
 * @author Alice Liang
 *
 * Created on Feb 25, 2019, 10:57 AM
 * 
 * @defgroup Stack Queue
 * @brief Stack and Queue for run operations
 */

#include <xc.h>
#include "stack.h"

struct stack{
    int pointer;//current stack pointer
    int array[14];//max 10 canisters has extra to prevent overflow
};

struct log{
    float dist;
    int serviced;
    int full;//1 for full 0 for empty
};

struct queue{
    int addP;//adding pointer
    int popP;//removing pointer
    struct log array[14];//array
};

//initiate stacks and queues
struct stack s;
struct queue q;
struct stack rtnS;

void initStacks(void){
    s.pointer=-1;
    q.addP=0;
    q.popP=0;
    rtnS.pointer=-1;
}

void push(int info){
    s.pointer=s.pointer+1;
    if ((s.pointer<14)&&(s.pointer>=0)){
        s.array[s.pointer]=info;   
    }
    if (info==1){//check on rtn open to left
        rtnS.pointer = rtnS.pointer + 1;
        rtnS.array[rtnS.pointer] = q.addP;
    }
}

int pop(void){
    int data;
    if (s.pointer>=0){
        data=s.array[s.pointer];
        s.array[s.pointer] = 0;
        s.pointer = s.pointer-1;
    }
    return data;
}

void enqueue(float dist, int ball, int full){
    q.array[q.addP].dist = dist;
    q.array[q.addP].serviced = ball;
    q.array[q.addP].full = full;
    q.addP = q.addP + 1;
}

void dequeue(float* dist, int* ball, int* full){
    if (q.popP<q.addP){
        *dist = q.array[q.popP].dist;
        q.array[q.popP].dist = 0;
        *ball = q.array[q.popP].serviced;
        q.array[q.popP].serviced = 0;
        *full = q.array[q.popP].full;
        q.array[q.popP].full = 0;
        q.popP = q.popP + 1;
    }
}

void adjQ(int ball, int full){//adjusts q for ball status on return journey
    int index;
    if (rtnS.pointer>=0){
        index=rtnS.array[rtnS.pointer];
        rtnS.array[rtnS.pointer] = 0;
        rtnS.pointer = rtnS.pointer-1;
    }
    q.array[index].serviced = ball;
    q.array[index].full = full;
}
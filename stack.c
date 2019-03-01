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
    int array[12];//max 10 canisters has extra to prevent overflow
};

struct log{
    int dist;
    int serviced;
    int full;//1 for full 0 for empty
};

struct queue{
    int addP;//adding pointer
    int popP;//removing pointer
    struct log array[12];//array
};

//initiate stacks and queues
struct stack s;
struct queue q;
struct stack rtnS;//canister locations in the queue for adjusting during returns

void initStacks(void){//initializes the starting pointers for each stack
    s.pointer=-1;
    q.addP=0;
    q.popP=0;
    rtnS.pointer=-1;
}

void push(int info,int open){
    s.pointer=s.pointer+1;//increment pointer
    if ((s.pointer<12)&&(s.pointer>=0)){
        s.array[s.pointer]=info;//add info to array 
    }
    if (open==1){//check on rtn open to left
        rtnS.pointer = rtnS.pointer + 1;//add the q location to rtnS
        rtnS.array[rtnS.pointer] = q.addP;
    }
}

int pop(void){
    int data;
    if (s.pointer>=0){
        data=s.array[s.pointer];//extract data
        s.array[s.pointer] = 0;//reset the array value
        s.pointer = s.pointer-1;//decrement pointer
    }
    return data;
}

void enqueue(int dist, int ball, int full){
    q.array[q.addP].dist = dist;//add data
    q.array[q.addP].serviced = ball;
    q.array[q.addP].full = full;
    q.addP = q.addP + 1;//increment pointer
}

void dequeue(int* dist, int* ball, int* full){
    if (q.popP<q.addP){
        *dist = q.array[q.popP].dist;//return data via pointers
        q.array[q.popP].dist = 0;//reset array value
        *ball = q.array[q.popP].serviced;
        q.array[q.popP].serviced = 0;
        *full = q.array[q.popP].full;
        q.array[q.popP].full = 0;
        q.popP = q.popP + 1;//increment pop pointer
    }
}

void adjQ(int ball, int full){//adjusts q for ball status on return journey
    int index;
    if (rtnS.pointer>=0){
        index=rtnS.array[rtnS.pointer];//extract index of q location to be adjusted
        rtnS.array[rtnS.pointer] = 0;//reset array value
        rtnS.pointer = rtnS.pointer-1;//decrement pointer
    }
    q.array[index].serviced = ball;//adjust the q values
    q.array[index].full = full;
}

int qLen(void){//returns the length of the queue
    return q.addP;
}
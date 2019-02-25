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
#include "File4.h"

struct stack{
    int pointer;//current stack pointer
    int array[14];//max 10 canisters has extra to prevent overflow
};

struct log{
    float dist;
    int serviced;
};

struct queue{
    int addP;//adding pointer
    int popP;//removing pointer
    struct log array[14];//array
};

//initiate stacks and queues
struct stack s;
struct queue q;
struct stack rtServiced;

void rstStack(void){
    s.pointer=0;
}

void push(int info){
    if ((s.pointer<14)&&(s.pointer>=0)){
        s.array[s.pointer]=info;
        s.pointer=s.pointer+1;
    }
}

int pop(void){
    int data;
    if (s.pointer>0){
        data=s.array[s.pointer];
        s.pointer = s.pointer-1;
    }
    else if(s.pointer == 0){
        data=s.array[s.pointer];
    }
    return data;
}
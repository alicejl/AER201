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

 /**
 * @brief struct for holding stack array and pointer
 */
struct stack;
struct log;
struct queue;

/**
 * @brief reset all pointers
 */
void initStacks(void);

/**
 * @brief push info onto s, pushes the end of q pointer onto rtn if open is 0 and can. opens to the left
 */
void push(int info,int open);

/**
 * @brief pops data off stack and decrements pointer
 */
int pop(void);

/** @brief enqueues data*/
void enqueue(int dist, int ball, int full);

/** @brief dequeues data*/
void dequeue(int* dist, int* ball, int* full);

/** @brief returns the length of the queue*/
int qLen(void);//returns addP

/**adjusts the ball and full values for the return trip*/
void adjQ(int ball, int full);
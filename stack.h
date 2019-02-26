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
 * @brief push info onto s, pushes the end of q pointer onto rtn if info is 2 and can. opens to the left
 */
void push(int info);

/**
 * @brief pops data off stack and decrements pointer
 */
int pop(void);

void enqueue(float dist, int ball, int full);
void dequeue(float* dist, int* ball, int* full);


void adjQ(int ball, int full);
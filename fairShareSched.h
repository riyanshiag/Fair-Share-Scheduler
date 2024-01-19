#ifndef _SCHED_FAIR_SHARE
#define _SCHED_FAIR_SHARE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define INT_MAX 2147483647
#define INT_MIN -2147483648
#define forn(k, n) for (int i = k; i < n; i++)
#define UPPER 100
#define LOWER 60

// Job definiton
typedef struct
{
    int jid; // Job ID
    int gid; // Group ID
    int arrivalTime;
    int basePriority;
    int calculatedPriority;
    int cpuIndex;
    int ioIndex;
    int bursts; // The number of CPU bursts for this job
    int *cpu;
    int *io;
    int cpuCount;
    int groupCount;
    bool wentForIO;
} Job;

// Queue implementation
struct ReadyQueue
{
    Job *job;
    struct ReadyQueue *next;
};

typedef struct ReadyQueue QNode;

/* This structure is used mainly for returning heads of ready queue and waiting queue if any
modifications are made to them while transfering jobs between the two queues*/
typedef struct
{
    QNode *q; QNode *wq;
} RES;

QNode *initQueue(Job *job);

void insertInQueue(QNode *head, Job *job);

// Utility function for debugging
void printQueue(QNode *q);

// Deletes a node "node" from the queue. Returns head of queue after deletion
QNode *deleteFromQueue(QNode *head, QNode *node);

// Job executions

// Picks next job to execute from ready queue
QNode *pickAJobToExecute(QNode *q);

// Returns heads of ready queue and waiting after executing the job
RES executeJob(QNode *q, QNode *wq, QNode *node, int *currentTime, int timeSlice, int prevDecisionPoint);

// Transfers any jobs that have finished IO from waiting queue to ready queue after every decision point
RES transferToReadyQueue(QNode* q, QNode* wq, int currentTime);

// Calculates priorities for all processes in ready queue after execution of a job
void calculatePriority(QNode *node, int groups);

// Finds next job to execute when CPU becomes idle and returns the arrival time of the next job
int findNextJob(QNode *q);

// Increases group count of all processes of same group after execution of a job in the same group
void increaseGroupCount(QNode *q, int gid, int execution);

// Returns the number of groups present in ready queue for calculating priority
int getNumberOfGroups(QNode *q, int prevDecisionPoint);

// Equates the group count of all group members in ready queue of the current executed process
void equateGroupCount(QNode *q, QNode *node);

// Adds group count for any newly arrived processes
void groupCountFunction(QNode *q);

// Set declarations
struct Group
{
    int gid;
    struct Group *next;
};

typedef struct
{
    int size;
    struct Group *next;
} Set;

Set *initSet();

void insertInSet(Set *set, int gid);

void printSet(Set *set);

void freeSet(Set *set);

#endif
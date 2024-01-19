#include "fairShareSched.h"

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("Invalid Arguments\n");
        return 1;
    }
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        printf("File doesn't exist\n");
        return 1;
    }
    if (argc == 2)
    {
        printf("Error: Time slice not defined\n");
        return 1;
    }
    srand(time(0));
    int timeSlice = atoi(argv[2]);
    if (timeSlice == 0)
    {
        printf("Error: Time Slice entered is invalid\n");
        return 1;
    }
    int n;
    fscanf(fp, "%d", &n);
    /*
    q represents the Ready Queue
    wq represents a queue for all processes that are to join the Ready Queue
    The waiting queue keeps a track of processes that will join the ready queue at a later time.
    */
    QNode *q = NULL, *wq = NULL;
    int totalExecutionTime = 0, currentTime = 0, prevDecisionPoint = 0;
    forn(0, n)
    {
        Job *job = (Job *)malloc(sizeof(Job));
        fscanf(fp, "%d", &job->gid);
        fscanf(fp, "%d", &job->basePriority);
        job->calculatedPriority = job->basePriority;
        job->jid = i + 1;
        fscanf(fp, "%d", &job->arrivalTime);
        int bursts;
        fscanf(fp, "%d", &bursts);
        job->bursts = bursts;
        job->ioIndex = 0;
        job->cpuIndex = 0;
        job->groupCount = 0;
        job->cpuCount = 0;
        job->cpu = malloc(sizeof(int) * bursts);
        job->io = malloc(sizeof(int) * bursts);
        job->wentForIO = false;
        for (int i = 0; i < bursts; i++)
            fscanf(fp, "%d", &job->cpu[i]);
        // Comment for loop for checking worst case time
        for (int i = 0; i < bursts; i++)
            job->cpu[i] = (((rand() % (UPPER - LOWER + 1)) + LOWER) * job->cpu[i]) / 100;
        for (int i = 0; i < bursts - 1; i++)
            fscanf(fp, "%d", &job->io[i]);
        if (job->arrivalTime > currentTime)
        {
            if (wq)
                insertInQueue(wq, job);
            else
                wq = initQueue(job);
        }
        else
        {
            if (q)
                insertInQueue(q, job);
            else
                q = initQueue(job);
        }
    }
    fclose(fp);
    while (q || wq)
    {
        QNode *node = pickAJobToExecute(q);
        if (node == NULL)
        {
            int nextJobArrivalTime = findNextJob(wq);
            printf("[%d]: CPU idle\n", currentTime);
            currentTime = nextJobArrivalTime;
            RES res = transferToReadyQueue(q, wq, currentTime);
            q = res.q;
            wq = res.wq;
        }
        else
        {
            printf("[%d]: Job %d executing\n", currentTime, node->job->jid);
            RES res = executeJob(q, wq, node, &currentTime, timeSlice, prevDecisionPoint);
            q = res.q;
            wq = res.wq;
            res = transferToReadyQueue(q, wq, currentTime);
            q = res.q;
            wq = res.wq;
            groupCountFunction(q);
        }
        prevDecisionPoint = currentTime;
    }
    printf("[%d]: Execution over\n", currentTime);
    return 0;
}
#include "fairShareSched.h"

int getNumberOfGroups(QNode *q, int prevDecisionPoint)
{
    Set *set = initSet();
    while (q)
    {
        if (q->job->arrivalTime <= prevDecisionPoint)
            insertInSet(set, q->job->gid);
        q = q->next;
    }
    int ans = set->size;
    freeSet(set);
    return ans;
}

QNode *pickAJobToExecute(QNode *q)
{
    QNode *ans = NULL;
    int priority = INT_MAX;
    while (q)
    {
        if (q->job->calculatedPriority < priority)
        {
            priority = q->job->calculatedPriority;
            ans = q;
        }
        q = q->next;
    }
    return ans;
}

void calculatePriority(QNode *node, int groups)
{
    node->job->cpuCount /= 2;
    node->job->groupCount /= 2;
    node->job->calculatedPriority = node->job->basePriority + (int)(node->job->cpuCount / 2) + (int)(node->job->groupCount * groups / 4);
}

void increaseGroupCount(QNode *q, int gid, int execution)
{
    while (q)
    {
        if (q->job->gid == gid)
            q->job->groupCount += execution;
        q = q->next;
    }
}

RES transferToReadyQueue(QNode *q, QNode *wq, int currentTime)
{
    RES res;
    res.wq = wq;
    res.q = q;
    QNode *prev = NULL, *temp = q;
    while (temp && temp->next)
        temp = temp->next;
    // Remove from Waiting Queue and add to Ready Queue
    while (wq)
    {
        if (wq->job->arrivalTime <= currentTime)
        {
            QNode *node = wq->next;
            wq->next = NULL;
            if (prev)
            {
                prev->next = node;
                if (temp)
                {
                    temp->next = wq;
                    temp = temp->next;
                }
                else
                {
                    res.q = wq;
                    temp = res.q;
                }
            }
            else
            {
                res.wq = node;
                if (temp)
                {
                    temp->next = wq;
                    temp = temp->next;
                }
                else
                {
                    res.q = wq;
                    temp = res.q;
                }
            }
            wq = node;
        }
        else
        {
            prev = wq;
            wq = wq->next;
        }
    }
    return res;
}

RES transferToWaitingQueue(QNode *q, QNode *wq, QNode *node)
{
    RES res;
    res.q = q;
    res.wq = wq;
    // Remove from Ready Queue and add to Waiting Queue
    QNode *prev = NULL, *temp = q;
    while (wq && wq->next)
        wq = wq->next;
    while (q)
    {
        if (q == node)
        {
            if (prev)
            {
                prev->next = node->next;
                node->next = NULL;
                if (res.wq)
                    wq->next = node;
                else
                    res.wq = node;
            }
            else
            {
                res.q = node->next;
                node->next = NULL;
                if (res.wq)
                    wq->next = node;
                else
                    res.wq = node;
            }
            break;
        }
        prev = q;
        q = q->next;
    }
    return res;
}

RES executeJob(QNode *q, QNode *wq, QNode *node, int *currentTime, int timeSlice, int prevDecisionPoint)
{
    RES res;
    res.q = q;
    res.wq = wq;
    int execution = node->job->cpu[node->job->cpuIndex];
    if (execution <= timeSlice)
    {
        node->job->cpu[node->job->cpuIndex] = 0;
        node->job->cpuIndex++;
        increaseGroupCount(q, node->job->gid, execution);
        *currentTime += execution;
        if (node->job->cpuIndex == node->job->bursts)
        {
            q = deleteFromQueue(q, node);
            res.q = q;
        }
        else
        {
            node->job->arrivalTime = *currentTime + node->job->io[node->job->ioIndex];
            printf("[%d]: Job %d gone for I/O. Will be back at %d\n", *currentTime, node->job->jid, node->job->arrivalTime);
            node->job->calculatedPriority = node->job->basePriority;
            node->job->ioIndex++;
            node->job->cpuCount = 0;
            node->job->groupCount = 0;
            node->job->wentForIO = true;
            res = transferToWaitingQueue(q, wq, node);
            q = res.q;
            wq = res.wq;
        }
    }
    else
    {
        execution = timeSlice;
        node->job->cpu[node->job->cpuIndex] -= timeSlice;
        increaseGroupCount(q, node->job->gid, execution);
        *currentTime += execution;
        node->job->cpuCount += execution;
    }
    int groups = getNumberOfGroups(q, prevDecisionPoint);
    QNode *temp = q;
    while (q)
    {
        if (q->job->arrivalTime <= prevDecisionPoint)
            calculatePriority(q, groups);
        q = q->next;
    }
    return res;
}

int findNextJob(QNode *q)
{
    int minTime = INT_MAX;
    while (q)
    {
        if (q->job->arrivalTime < minTime)
            minTime = q->job->arrivalTime;
        q = q->next;
    }
    return minTime;
}

void equateGroupCount(QNode *q, QNode *node)
{
    while (q)
    {
        if (q->job->gid == node->job->gid && q->job->wentForIO == false)
        {
            node->job->groupCount = q->job->groupCount;
            return;
        }
        q = q->next;
    }
}

void groupCountFunction(QNode *q)
{
    QNode *temp = q;
    while (q)
    {
        if (q->job->wentForIO)
        {
            // Find other process that hasn't gone for I/O and equate group count
            // Else it's group count is 0
            q->job->groupCount = 0;
            equateGroupCount(temp, q);
            q->job->wentForIO = false;
        }
        q = q->next;
    }
}

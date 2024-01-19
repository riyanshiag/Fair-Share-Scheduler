#include "fairShareSched.h"

QNode *initQueue(Job *job)
{
    QNode *head = (QNode *)malloc(sizeof(QNode));
    head->next = NULL;
    head->job = job;
    return head;
}

void insertInQueue(QNode *head, Job *job)
{
    while (head->next != NULL)
        head = head->next;
    head->next = (QNode *)malloc(sizeof(QNode));
    head = head->next;
    head->job = job;
    head->next = NULL;
}

void printQueue(QNode *q)
{
    while (q)
    {
        printf("Job %d, Group ID: %d, Arrival Time: %d, Priority: %d\n", q->job->jid, q->job->gid, q->job->arrivalTime, q->job->calculatedPriority);
        printf("Actual CPU bursts: ");
        for (int i = 0; i < q->job->bursts; i++)
            printf("%d ", q->job->cpu[i]);
        printf("\n");
        if (q->job->bursts - 1)
        {
            printf("IO bursts: ");
            for (int i = 0; i < q->job->bursts - 1; i++)
                printf("%d ", q->job->io[i]);
            printf("\n");
        }
        q = q->next;
    }
    printf("\n");
}

QNode *deleteFromQueue(QNode *head, QNode *node)
{
    free(node->job->cpu);
    free(node->job->io);
    free(node->job);
    QNode *prev = NULL, *newHead = head;
    while (head)
    {
        if (head == node)
        {
            if (prev == NULL)
            {
                QNode *newHead = head->next;
                return newHead;
            }
            else
            {
                prev->next = head->next;
                return newHead;
            }
        }
        prev = head;
        head = head->next;
    }
    free(node);
}
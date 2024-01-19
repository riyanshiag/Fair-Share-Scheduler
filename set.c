#include "fairShareSched.h"

Set *initSet()
{
    Set *set = (Set *)malloc(sizeof(Set));
    set->size = 0;
    set->next = NULL;
    return set;
}

void printSet(Set *set)
{
    printf("Group Size: %d\n", set->size);
    struct Group *temp = set->next;
    while (temp)
    {
        printf("%d ", temp->gid);
        temp = temp->next;
    }
}

void insertInSet(Set *set, int gid)
{
    struct Group *temp = set->next;
    if (temp == NULL)
    {
        set->next = (struct Group *)malloc(sizeof(struct Group));
        set->next->gid = gid;
        set->next->next = NULL;
        set->size++;
        return;
    }
    while (temp->next)
    {
        if (temp->gid == gid)
            return;
        temp = temp->next;
    }
    if (temp->gid == gid)
        return;
    struct Group *newNode = (struct Group *)malloc(sizeof(struct Group));
    temp->next = newNode;
    newNode->gid = gid;
    newNode->next = NULL;
    set->size++;
    return;
}

void freeSet(Set *set)
{
    struct Group *groups = set->next, *temp = NULL;
    free(set);
    while (groups)
    {
        temp = groups->next;
        free(groups);
        groups = temp;
    }
}
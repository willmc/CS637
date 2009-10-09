#include "traceproc.h"
#include "types.h"
#include "stat.h"
#include "spalloc.h"

void addProc(int i)
{
    proc_node* newProc;
    //newProc = (proc_node*)malloc(sizeof(proc_node));
    char *newmem;
    newmem = spalloc(32);
    /*
    newmem = kalloc(4096);
    if(newmem == 0)
    {
        cprintf("b0rked\n");
        return;
    }
    memmove(newmem, newProc->mem, 4096);
    //kfree(newProc->mem, 4096);

    //newProc->mem = kalloc(4096);
    */
    newProc = (proc_node*) newmem;
    newProc->pid = i;
    newProc->next = 0; 

    if(hasOneProc == 0)
    {
        head_proc_node = newProc;    
        curr_proc_node = newProc;
        hasOneProc = 1;
        numProcs = 1;
    }
    else
    {
        curr_proc_node->next = newProc;
        curr_proc_node = newProc;
        curr_proc_node->next = 0;
        numProcs++;
    }
    newProc->mem = newmem;
}

void printProcList()
{
    proc_node *runThroughProcList;
    runThroughProcList = head_proc_node;

    cprintf("Printing out all PIDs that were on the CPU.\n");
    int i = 0;
    while(i < numProcs)
    {
        cprintf( "%d\n", runThroughProcList->pid);
        runThroughProcList = runThroughProcList->next;
        i++;
    }
    cprintf("Done printing PIDs.\n");
}

void clearProcList()
{
    proc_node* currNode;
    proc_node* nextNode;

    currNode = head_proc_node;

    while(currNode)
    {
        nextNode = currNode->next;
        //free(currNode);
        kfree(currNode->mem, 4096);
        currNode = nextNode;
    }

    hasOneProc = 0;
}

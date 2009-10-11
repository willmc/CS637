#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  int pid;
  struct proc *np;

  if((np = copyproc(cp)) == 0)
    return -1;
  pid = np->pid;
  //np->state = RUNNABLE;

#ifdef LOTTERY
  np->tickets = NEW_PROC_TICKETS;
#endif
  chngstate(np, RUNNABLE);
  return pid;
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return cp->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  if((addr = growproc(n)) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n, ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(cp->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

int 
sys_tickcnt(void)
{
    return ticks; 
}

extern struct proc proc[NPROC];

int 
sys_get_tickets()
{
#ifdef LOTTERY
    int pid;
    if(argint(0, &pid) < 0)
        return -1;
    if(pid >= NPROC || pid < 0)
        return -2;
    return proc[pid].tickets;
#else
    return -3;
#endif
}

int
sys_set_tickets()
{
#ifdef LOTTERY
    int pid, newticket;
    if(argint(0, &pid) || argint(1,&newticket))
        return -1;
    if(pid >= NPROC || pid < 0)
        return -1;

    set_tickets(pid, newticket);

    return 0;
#else
    return -1;
#endif
}


int 
sys_gettimesrun()
{
    int pid, timesRun;
    if(argint(0, &pid) < 0)
        return -1;
    if(pid >= NPROC || pid < 0)
        return -1;

    int i;
    for( i = 0; i < NPROC; i++)
    {
       if(proc[i].pid == pid)
           return proc[i].timesRun;
    }
}

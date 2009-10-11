#include "types.h"
#include "param.h"
#include "user.h"

int main(int argc, char *argv[])
{

    if(argc != 2)
    {
        printf(1,"Needs 2 arguments!\n");
        exit();
    }

    int ratio = atoi(argv[1]);
    
    set_tickets(getpid(), ratio);

    printf(1, "expected ratio: %d:1.\n", ratio);
    int forkretv = fork();

    if(forkretv == 0)
    {
        set_tickets(getpid(), 1);
    }

    int j = 0;
    int k = 1;
    int startTime = tickcnt();

    while(startTime + 6000 > tickcnt())  
    {
        k = (startTime + j) / 133;
        j++;
        k--;

        //printf(1, "k = %d\n", k);
    }

    int kidsPid;

    if(forkretv > 0)
        kidsPid = wait();

    if(forkretv == 0)
    {
        printf(1,"c ran %d times.\n", gettimesrun(getpid()));
        exit();
    }

    printf(1,"p ran %d times.\n", gettimesrun(getpid()));

    exit();
}

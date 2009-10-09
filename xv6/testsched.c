#include "types.h"
#include "stat.h"
#include "user.h"

void computeCrap();

int main(int argc, char *argv[])
{
    int k;

    if(argc != 3)
    {
        printf(2,"Needs 2 arguments.\n");
        exit();
    }

    int numForks = atoi(argv[1]);
    int lim = 1 << atoi(argv[2]);
    int j;
    for(j = 0; j < numForks; j++)
    {
        fork();
    }

    int i;
    for(i = 0; i < lim; i++)
    {
        k = i / 12345;
    }

    exit();
    /*
    pid = fork();
    //is it us?
    if(pid == 0)
    {
        for(int i = 0; i < 100; i++)
        {
            k = i / 777;
        }
        exit();
    }
    else if( pid > 0)
    {
        for(int i = 0; i < 100; i++)
        {
            k = i / 777;
        }
        exit();
    }
    */
}


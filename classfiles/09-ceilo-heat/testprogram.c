
/**********************************************************************

  Program creates arrays of several megabytes and accesses them in a
  loop, thereby creating memory pressure.

  Command line arguments: Number of megabytes, and number of loops.
  A loop number of 0 means an infinite loop.

  The program puts itself in the background and closes all file
  descriptors. This way, it can be conveniently launched on a remote
  system using ssh.

 **********************************************************************/
#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include <stdlib.h>

#define MB (1024*1024)

char *arrays[1024*1024];

main(int argc, char *argv[])
{
  int f,loop,mb;

  int megabytes = 256;
  int nloops = 200;
  int infinite = 0;     /*Boolean*/

    if (argc>1)
    {
        if (argv[1][0]=='?')
        {
            printf("testprogram [<size in MB>] [<loops>]\n");
            printf("For infinite looping, set <loops> to 0\n");
            exit(0);
        }
        else
            megabytes = atoi(argv[1]);
    }
    if (argc>2)
        nloops = atoi(argv[2]);

    /* Run in the background */
    f = fork();
    if (f<0)
    {
        perror("Can't fork");
        exit(1);
    }
    else if (f>0)
    {
        printf("created child %d; bye!\n",f);
        exit(0);    /* Parent exits */
    }

    /* Allocate the memory */
    for (mb=0; mb<megabytes; mb++)
    {
       /* printf("allocating MB %d\n",mb); */
       arrays[mb] = malloc(MB);
       if ( arrays[mb]==NULL )
       {
           perror("Can't allocate array");
           exit(1);
       }
    }

    /* close file descriptors so that ssh returns immediately */
    close(0);
    close(1);
    close(2);

    /* Access memory */
    loop = 0;
    while (1)
    {
        loop++;
        if (nloops && loop>=nloops)
                break;
        /* if nloops==0 continue looping forever */

        for (mb=0;mb<megabytes;mb++)
        {
          int i;
            /* printf("Loop %d, Array %d\n",loop,mb); */
            for (i=0;i<MB;i+=4096)
                arrays[mb][i] = 9;
        }

    }
}




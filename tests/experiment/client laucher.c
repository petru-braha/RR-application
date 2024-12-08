#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

void *launch_cl0(void *)
{
    execl("cl0", "cl0", NULL);
    return NULL;
}

void *launch_cl1(void *)
{
    execl("cl1", "cl1", NULL);
    return NULL;
}

int main()
{
    // for (int i = 0; i < 30; i++)
    {
        //printf("iteration: %d.\n", i);
        pthread_t th0, th1;
        pthread_create(&th0, NULL, &launch_cl0, NULL);
        pthread_create(&th1, NULL, &launch_cl1, NULL);

        pthread_join(th1, NULL);
    }

    return 0;
}

// first launch cl0
// then launch cl1 concurentlly
// wait for both here
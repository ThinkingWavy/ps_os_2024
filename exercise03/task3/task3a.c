#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>

int8_t counter = 1;

void *thread_func()
{
    counter += 1;
    pthread_exit(NULL);
}

int main()
{

#ifdef DEBUG
    printf("Counting just with Processes\n\n");
#endif

    /*Here the fork section starts*/
    printf("Before Fork: Counter value is %d\n", counter);
    pid_t pid;
    pid = fork();
    if (pid == 0)
    {
        counter += 1;
    }
    else if (pid < 0)
    {
        fprintf(stderr, "unable to create child processes");
        exit(EXIT_FAILURE);
    }
    else
    {
        wait(NULL);
    }
    printf("After Fork: Counter value is %d \n", counter);

/*Here the thread section starts*/
#if !defined(DEBUG)
    pthread_t ptid;
    if (pthread_create(&ptid, NULL, &thread_func, NULL) != 0)
    {
        fprintf(stderr, "Error while creating pthread!");
        exit(EXIT_FAILURE);
    }
    pthread_join(ptid, NULL);
#endif

    printf("End of process %d:  Counter value: %d\n", getpid(), counter);

    /*EXPLANATION:
    The global variable gets copied while forking and it is not the same reference.
    So when it is incremented in the child process it is not incremented in the parent process
    The second thread increments the global variable in each process seperatly again
    So the parent process has a counter of 2 in the end.
    And the child process has a counter of 3 in the end */
}
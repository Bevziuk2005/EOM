#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/* Завдання 14 (друга частина): той самий zombie.c, але parent
   робить wait(NULL), тому запис про child прибирається.
   Щоб встигнути подивитись на стан, parent спить 10 с ДО wait(). */

int main(void)
{
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        printf("CHILD: PID=%d, exiting now\n", getpid());
        return 0;
    }

    printf("PARENT: PID=%d, child=%d\n",
           getpid(),
           pid);
    fflush(stdout);

    sleep(10);

    wait(NULL);

    printf("PARENT: wait() done, child collected\n");
    fflush(stdout);

    sleep(10);

    return 0;
}

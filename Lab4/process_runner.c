#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* Фінальне завдання: маленький запускач програм.
   ./process_runner PROGRAM [аргументи...]

   fork() -> exec() у child -> waitpid() у parent -> розбір статусу.
   Приблизно так працює shell, коли ми вводимо команду. */

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s PROGRAM [args...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("Parent PID: %d\n", getpid());
    printf("Creating child...\n\n");
    fflush(stdout);   /* щоб буфер не потрапив у child разом з fork() */

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        /* child */
        printf("Child PID: %d\n", getpid());
        printf("Child PPID: %d\n", getppid());
        printf("Executing: %s\n\n", argv[1]);
        fflush(stdout);

        execvp(argv[1], &argv[1]);

        /* сюди потрапляємо лише якщо exec не вдався */
        fprintf(stderr, "exec %s: %s\n", argv[1], strerror(errno));
        _exit(127);
    }

    /* parent */
    int status;

    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return EXIT_FAILURE;
    }

    printf("\nChild %d completed\n", pid);

    if (WIFEXITED(status)) {
        printf("Exit status: %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("Killed by signal: %d (%s)\n",
               WTERMSIG(status),
               strsignal(WTERMSIG(status)));
    } else {
        printf("Child ended in an unexpected way (raw status=%d)\n", status);
    }

    printf("\nParent continues\n");

    return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/* Завдання 6: ls -l | wc -l без shell, руками через pipe + dup2 + exec */

int main(void)
{
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    pid_t child1 = fork();

    if (child1 == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (child1 == 0) {
        /* Child 1: stdout -> pipe */
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            _exit(EXIT_FAILURE);
        }

        close(pipefd[0]);
        close(pipefd[1]);

        char *args[] = {"ls", "-l", NULL};

        execvp(args[0], args);
        perror("execvp ls");
        _exit(EXIT_FAILURE);
    }

    pid_t child2 = fork();

    if (child2 == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (child2 == 0) {
        /* Child 2: stdin <- pipe */
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            _exit(EXIT_FAILURE);
        }

        close(pipefd[0]);
        close(pipefd[1]);

        char *args[] = {"wc", "-l", NULL};

        execvp(args[0], args);
        perror("execvp wc");
        _exit(EXIT_FAILURE);
    }

    /* Parent у pipe не читає і не пише - обидва кінці закриваємо.
       Інакше wc ніколи не дочекається EOF. */
    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);

    return EXIT_SUCCESS;
}

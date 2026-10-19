#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* Завдання 3: що буде, якщо не закрити зайвий кінець pipe.
   ./pipe_no_close bad   - child НЕ закриває свій write end -> read() ніколи не поверне 0
   ./pipe_no_close good  - child закриває write end -> є EOF, цикл завершується */

int main(int argc, char *argv[])
{
    int close_write_in_child = 1;

    if (argc == 2 && strcmp(argv[1], "bad") == 0) {
        close_write_in_child = 0;
    }

    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        if (close_write_in_child) {
            close(pipefd[1]);
        }

        char buffer[128];
        ssize_t n;

        printf("[child %d] reading until EOF...\n", getpid());
        fflush(stdout);

        while ((n = read(pipefd[0],
                         buffer,
                         sizeof(buffer))) > 0) {
            printf("[child] got %zd bytes\n", n);
            fflush(stdout);
        }

        if (n == 0) {
            printf("[child] EOF\n");
        } else {
            perror("read");
        }

        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    }

    close(pipefd[0]);

    const char message[] = "one message\n";

    if (write(pipefd[1], message, strlen(message)) == -1) {
        perror("write");
    }

    close(pipefd[1]);   /* parent свій write end закрив */

    printf("[parent %d] write end closed, waiting for child\n", getpid());
    fflush(stdout);

    waitpid(pid, NULL, 0);

    return EXIT_SUCCESS;
}

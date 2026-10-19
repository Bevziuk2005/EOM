#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* Завдання 8: parent через 2 с шле child-у сигнал.
   ./signal_parent_child default   - child без handler: SIGTERM його вбиває
   ./signal_parent_child handler   - child має свій handler для SIGTERM
   ./signal_parent_child kill      - parent шле SIGKILL, handler не допоможе */

static void handle_sigterm(int signal_number)
{
    (void)signal_number;

    const char message[] =
        "[child] SIGTERM caught, finishing normally\n";

    write(STDOUT_FILENO, message, sizeof(message) - 1);

    _exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    const char *mode = argc == 2 ? argv[1] : "default";
    int use_handler = strcmp(mode, "default") != 0;
    int sig = strcmp(mode, "kill") == 0 ? SIGKILL : SIGTERM;

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        if (use_handler) {
            struct sigaction action = {0};

            action.sa_handler = handle_sigterm;
            sigemptyset(&action.sa_mask);

            if (sigaction(SIGTERM, &action, NULL) == -1) {
                perror("sigaction");
                _exit(EXIT_FAILURE);
            }
        }

        printf("[child %d] waiting for a signal (mode: %s)\n", getpid(), mode);
        fflush(stdout);

        while (1) {
            pause();
        }
    }

    sleep(2);

    printf("[parent] sending signal %d (%s) to child %d\n",
           sig, strsignal(sig), pid);
    fflush(stdout);

    kill(pid, sig);

    int status;

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return EXIT_FAILURE;
    }

    if (WIFSIGNALED(status)) {
        printf("[parent] child killed by signal %d (%s)\n",
               WTERMSIG(status), strsignal(WTERMSIG(status)));
    } else if (WIFEXITED(status)) {
        printf("[parent] child exited normally, code %d\n",
               WEXITSTATUS(status));
    }

    return EXIT_SUCCESS;
}

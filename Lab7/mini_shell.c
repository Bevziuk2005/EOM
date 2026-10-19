#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* Фінальне завдання: mini-shell.
   Вміє: зовнішні команди, built-in cd/exit, одне перенаправлення ">",
   один pipe "a | b". Токени розділяються пробілами (без лапок).
   Ctrl+C: shell ігнорує SIGINT, а child перед exec повертає SIG_DFL. */

#define MAX_LINE 1024
#define MAX_ARGS 64

/* Чекаємо конкретного child; перерваний сигналом waitpid повторюємо */
static int wait_for(pid_t pid)
{
    int status;

    while (waitpid(pid, &status, 0) == -1) {
        if (errno == EINTR) {
            continue;
        }
        perror("waitpid");
        return -1;
    }

    if (WIFSIGNALED(status)) {
        /* команду вбито сигналом, наприклад Ctrl+C */
        fprintf(stderr, "[%s]\n", strsignal(WTERMSIG(status)));
    }

    return status;
}

/* Child: повернути типову реакцію на SIGINT і запустити команду */
static void run_child(char *argv[])
{
    signal(SIGINT, SIG_DFL);

    execvp(argv[0], argv);

    perror("execvp");
    _exit(EXIT_FAILURE);
}

/* Проста команда, можливо з "> file" */
static void run_simple(char *argv[], const char *outfile)
{
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        if (outfile != NULL) {
            int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

            if (fd == -1) {
                perror("open");
                _exit(EXIT_FAILURE);
            }

            if (dup2(fd, STDOUT_FILENO) == -1) {
                perror("dup2");
                _exit(EXIT_FAILURE);
            }

            close(fd);
        }

        run_child(argv);
    }

    wait_for(pid);
}

/* left | right */
static void run_pipeline(char *left[], char *right[])
{
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    pid_t child1 = fork();

    if (child1 == -1) {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return;
    }

    if (child1 == 0) {
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            _exit(EXIT_FAILURE);
        }

        close(pipefd[0]);
        close(pipefd[1]);
        run_child(left);
    }

    pid_t child2 = fork();

    if (child2 == -1) {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        wait_for(child1);
        return;
    }

    if (child2 == 0) {
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            _exit(EXIT_FAILURE);
        }

        close(pipefd[0]);
        close(pipefd[1]);
        run_child(right);
    }

    /* parent у pipe не бере участі: закриваємо обидва кінці */
    close(pipefd[0]);
    close(pipefd[1]);

    wait_for(child1);
    wait_for(child2);
}

int main(void)
{
    /* shell не повинен вмирати від Ctrl+C */
    struct sigaction ignore = {0};

    ignore.sa_handler = SIG_IGN;
    sigemptyset(&ignore.sa_mask);
    sigaction(SIGINT, &ignore, NULL);

    char line[MAX_LINE];

    while (1) {
        printf("myshell> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            if (ferror(stdin) && errno == EINTR) {
                clearerr(stdin);
                printf("\n");
                continue;
            }
            printf("\n");
            break;                         /* Ctrl+D */
        }

        line[strcspn(line, "\n")] = '\0';

        /* розбір на токени */
        char *tokens[MAX_ARGS];
        int count = 0;
        char *token = strtok(line, " \t");

        while (token != NULL && count < MAX_ARGS - 1) {
            tokens[count++] = token;
            token = strtok(NULL, " \t");
        }

        tokens[count] = NULL;

        if (count == 0) {
            continue;
        }

        /* шукаємо "|" та ">" */
        int pipe_at = -1;
        int redirect_at = -1;

        for (int i = 0; i < count; i++) {
            if (strcmp(tokens[i], "|") == 0 && pipe_at == -1) {
                pipe_at = i;
            } else if (strcmp(tokens[i], ">") == 0 && redirect_at == -1) {
                redirect_at = i;
            }
        }

        /* built-in: exit і cd виконує сам shell, без fork */
        if (strcmp(tokens[0], "exit") == 0) {
            break;
        }

        if (strcmp(tokens[0], "cd") == 0) {
            if (count < 2) {
                fprintf(stderr, "cd: потрібен шлях\n");
            } else if (chdir(tokens[1]) == -1) {
                perror("cd");
            }
            continue;
        }

        if (pipe_at != -1) {
            if (redirect_at != -1) {
                fprintf(stderr, "myshell: pipe разом з > не підтримується\n");
                continue;
            }

            if (pipe_at == 0 || pipe_at == count - 1) {
                fprintf(stderr, "myshell: порожня команда біля |\n");
                continue;
            }

            tokens[pipe_at] = NULL;
            run_pipeline(tokens, &tokens[pipe_at + 1]);
            continue;
        }

        if (redirect_at != -1) {
            if (redirect_at == 0 || redirect_at != count - 2) {
                fprintf(stderr, "myshell: очікується: команда > файл\n");
                continue;
            }

            char *outfile = tokens[redirect_at + 1];

            tokens[redirect_at] = NULL;
            run_simple(tokens, outfile);
            continue;
        }

        run_simple(tokens, NULL);
    }

    return EXIT_SUCCESS;
}

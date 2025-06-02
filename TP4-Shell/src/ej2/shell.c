#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>

#define MAX_ARGS 64
#define MAX_CMDS 256

// valida comillas, pipes mal puestos, || y max args
int validar_sintaxis(const char *s) {
    int in_s = 0, in_d = 0;
    const char *p = s;
    if (*p == '|' || p[strlen(p) - 1] == '|') {
        fprintf(stderr, "syntax error: pipe al inicio o al final\n");
        return 0;
    }
    if (strstr(s, "||")) {
        fprintf(stderr, "syntax error: tuberia vacia (\"||\")\n");
        return 0;
    }
    for (; *p; p++) {
        if (*p == '\'' && !in_d) in_s = !in_s;
        else if (*p == '"' && !in_s) in_d = !in_d;
    }
    if (in_s || in_d) {
        fprintf(stderr, "syntax error: comillas sin cerrar\n");
        return 0;
    }
    in_s = in_d = 0;
    const char *start = s;
    p = s;
    while (1) {
        if (*p == '\'' && !in_d) in_s = !in_s;
        else if (*p == '"' && !in_s) in_d = !in_d;
        if ((*p == '|' && !in_s && !in_d) || *p == '\0') {
            int tokens = 0, in_tok = 0;
            for (const char *q = start; q < p; q++) {
                if (isspace((unsigned char)*q)) in_tok = 0;
                else if (!in_tok) { in_tok = 1; tokens++; }
            }
            if (tokens > MAX_ARGS) {
                fprintf(stderr, "error: demasiados argumentos\n");
                return 0;
            }
            if (*p == '\0') break;
            start = p + 1;
        }
        p++;
    }
    return 1;
}

// divide linea en comandos separados por |, ignora | en comillas
int dividir_en_comandos(char *linea, char *comandos[]) {
    int in_s = 0, in_d = 0, n = 0;
    char *start = linea;
    for (char *p = linea; ; p++) {
        if (*p == '\'' && !in_d) in_s = !in_s;
        else if (*p == '"' && !in_s) in_d = !in_d;
        if ((*p == '|' && !in_s && !in_d) || *p == '\0') {
            char *seg = start;
            while (*seg && isspace((unsigned char)*seg)) seg++;
            char *end = p - 1;
            while (end >= seg && isspace((unsigned char)*end)) {
                *end = '\0';
                end--;
            }
            if (*seg == '\0') {
                fprintf(stderr, "syntax error: subcomando vacio\n");
                return -1;
            }
            comandos[n++] = seg;
            if (*p == '\0') break;
            start = p + 1;
        }
    }
    return n;
}

// parsea cmd en argv[], mantiene comillas
char **parsear_comando(const char *cmd) {
    char **argv = malloc(sizeof(char*) * (MAX_ARGS + 2));
    char *buf = strdup(cmd);
    int argc = 0;
    char *p = buf;
    while (*p) {
        while (isspace((unsigned char)*p)) p++;
        if (!*p) break;
        if (*p == '\'' || *p == '"') {
            char quote = *p++;
            char *start = p;
            while (*p && *p != quote) p++;
            if (*p) *p++ = '\0';
            argv[argc++] = start;
        } else {
            char *start = p;
            while (*p && !isspace((unsigned char)*p)) p++;
            if (*p) *p++ = '\0';
            argv[argc++] = start;
        }
        if (argc == MAX_ARGS) break;
    }
    argv[argc] = NULL;
    return argv;
}

// ejecuta n comandos con pipes
void ejecutar_pipeline(char *comandos[], int n) {
    int pipes[MAX_CMDS - 1][2];
    for (int i = 0; i < n - 1; i++) pipe(pipes[i]);
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid < 0) exit(1);
        if (pid == 0) {
            if (i > 0) dup2(pipes[i - 1][0], STDIN_FILENO);
            if (i < n - 1) dup2(pipes[i][1], STDOUT_FILENO);
            for (int j = 0; j < n - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            char **argv = parsear_comando(comandos[i]);
            if (argv[0] == NULL) exit(0);
            if (strcmp(argv[0], "exit") == 0) exit(0);
            execvp(argv[0], argv);
            perror(argv[0]);
            exit(1);
        }
    }
    for (int i = 0; i < n - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    for (int i = 0; i < n; i++) wait(NULL);
}

int main() {
    char *line = NULL;
    size_t len = 0;
    while (1) {
        if (isatty(STDIN_FILENO)) {
            printf("Shell> ");
            fflush(stdout);
        }
        if (getline(&line, &len, stdin) == -1) break;
        size_t L = strlen(line);
        if (L > 0 && line[L - 1] == '\n') line[L - 1] = '\0';
        char *p = line;
        while (*p && isspace((unsigned char)*p)) p++;
        if (*p == '\0') continue;
        if (!validar_sintaxis(p)) continue;
        char *comandos[MAX_CMDS];
        int n = dividir_en_comandos(p, comandos);
        if (n < 0) continue;
        if (n == 1 && strcmp(comandos[0], "exit") == 0) break;
        ejecutar_pipeline(comandos, n);
    }
    free(line);
    return 0;
}

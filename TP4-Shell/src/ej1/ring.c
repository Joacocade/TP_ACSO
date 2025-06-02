#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char **argv) {
	if (argc != 4) {
		fprintf(stderr, "Uso: %s <n> <c> <s>\n", argv[0]);
		exit(1);
	}

    int n = atoi(argv[1]), c = atoi(argv[2]), s = atoi(argv[3]);
    int p2c[2], c2p[2], child[n][2];

    pipe(p2c);
    pipe(c2p);
    for (int i = 0; i < n; i++)
        if (i != s)
            pipe(child[i]);

    int before = (s + n - 1) % n;

    for (int i = 0; i < n; i++) {
        if (fork() == 0) {
            int in = (i == s ? p2c[0] : child[i][0]);
            int out = (i == before ? c2p[1] : child[(i + 1) % n][1]);

            // Cerrar descriptores no usados
            for (int k = 0; k < n; k++) {
                if (k != s) {
                    if (child[k][0] != in)  close(child[k][0]);
                    if (child[k][1] != out) close(child[k][1]);
                }
            }
            if (p2c[0] != in)  close(p2c[0]);
            if (p2c[1] != out) close(p2c[1]);
            if (c2p[0] != in)  close(c2p[0]);
            if (c2p[1] != out) close(c2p[1]);

            int v;
            read(in, &v, sizeof(int));
            v++;
            write(out, &v, sizeof(int));
            exit(0);
        }
    }

    // Padre: cerrar extremos de pipes que no usará
    for (int i = 0; i < n; i++) {
        if (i != s) {
            close(child[i][0]);
            close(child[i][1]);
        }
    }
    close(p2c[0]);
    close(c2p[1]);

    // Iniciar el anillo
    write(p2c[1], &c, sizeof(int));
    close(p2c[1]);

    int result;
    read(c2p[0], &result, sizeof(int));
    printf("Resultado final recibido en el padre: %d\n", result);

    while (wait(NULL) > 0);
    return 0;
}

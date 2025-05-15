
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    if (!fs || fs->dfd < 0 || !pathname || pathname[0] != '/') return -1;

    int inumber = ROOT_INUMBER;

    // saltamos el primer '/' del pathname
    pathname++;

    // si es solamente '/' retorna el inode raíz
    if (*pathname == '\0') return inumber;

    char component[15];  // 14 chars + '\0'
    const char *start = pathname;
    const char *end;

    while (1) { // encuentra el siguiente '/' o el final de la cadena
        end = strchr(start, '/');
        int len = end ? (end - start) : strlen(start);

        if (len == 0) { // ignoramos componentes vacios ("//")
            if (!end) break;
            start = end + 1;
            continue;
        }
        if (len > 14) return -1;  // nombre muy largo

        strncpy(component, start, len);
        component[len] = '\0';

        // manejar "." y ".."
        if (strcmp(component, ".") == 0) { } // no cambia inumber
        else if (strcmp(component, "..") == 0) { // no hay referencia a padre
            return -1;
        } else {
            struct direntv6 dirent;
            if (directory_findname(fs, component, inumber, &dirent) < 0)
                return -1;
            inumber = dirent.d_inumber;
        }

        if (!end) break;
        start = end + 1;

        if (*start == '\0') break;
    }

    return inumber;
}

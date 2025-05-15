#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int directory_findname(struct unixfilesystem *fs, const char *name, int dirinumber,
                        struct direntv6 *dirEnt) {

    if (!fs || fs->dfd < 0 || !name || !dirEnt) return -1;

    struct inode dir_inode;
    if (inode_iget(fs, dirinumber, &dir_inode) < 0) return -1;

    // vemos si esta asignado y que sea un directorio
    if (!(dir_inode.i_mode & IALLOC) || (dir_inode.i_mode & IFMT) != IFDIR)
        return -1;

    // calculamos tamaño y bloques a recorrer
    int size = inode_getsize(&dir_inode);
    if (size < 0) return -1;  // inode invalido
    int numBlocks = (size + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;
    char buf[DISKIMG_SECTOR_SIZE];

    for (int bno = 0; bno < numBlocks; bno++) {
        int bytesRead = file_getblock(fs, dirinumber, bno, buf);
        if (bytesRead < 0) return -1;

        int entries = bytesRead / sizeof(struct direntv6);
        struct direntv6 *entries_ptr = (struct direntv6 *) buf;

        // comparamos cada nombre con el buscado
        for (int i = 0; i < entries; i++) {
            if (strncmp(entries_ptr[i].d_name, name, 14) == 0) {
                *dirEnt = entries_ptr[i];  // copia la entrada encontrada
                return 0;
            }
        }
    }

    return -1;
}

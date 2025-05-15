#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    if (!fs || fs->dfd < 0 || !buf) return -1;

    struct inode in;
    if (inode_iget(fs, inumber, &in) < 0) return -1;

    int filesize = inode_getsize(&in);
    if (filesize < 0) return -1;  // inode no valido
    int totalBlocks = (filesize + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;
    if (blockNum >= totalBlocks) return -1;  // bloque fuera de rango

    // conseguimos el numero de sector correspondiente
    int sector = inode_indexlookup(fs, &in, blockNum);
    if (sector < 0) return -1;

    if (diskimg_readsector(fs->dfd, sector, buf) != DISKIMG_SECTOR_SIZE)
        return -1;

    if (blockNum == totalBlocks - 1 && filesize % DISKIMG_SECTOR_SIZE != 0)
        return filesize % DISKIMG_SECTOR_SIZE;

    return DISKIMG_SECTOR_SIZE;
}

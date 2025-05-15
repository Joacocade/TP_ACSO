#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"


int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    if (!fs || fs->dfd < 0 || inumber < 1 || !inp) return -1;

    int inodeSector = INODE_START_SECTOR + (inumber - 1) / 16;
    char buf[DISKIMG_SECTOR_SIZE];

    // leemos todo el sector de inodes
    if (diskimg_readsector(fs->dfd, inodeSector, buf) != DISKIMG_SECTOR_SIZE) {
        return -1;  // error de lectura
    }

    // calculamos el desplazamiento dentro del sector (0..15)
    int offset = (inumber - 1) % 16;
    struct inode *inodes = (struct inode *) buf;

    // copiamos el inode solicitado al buffer de salida
    *inp = inodes[offset];
    return 0;
}

int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
    if (!fs || fs->dfd < 0 || !inp || blockNum < 0) return -1;

    // archivo chico
    if (!(inp->i_mode & ILARG)) {
        // max 8 bloques directos
        if (blockNum >= 8) return -1;
        int sector = inp->i_addr[blockNum];
        // error si no hay sector asignado
        return (sector == 0) ? -1 : sector;
    }

    // archivo grande, primeros 7 bloques son indirectos simples
    if (blockNum < 7 * 256) {
        int indir_index = blockNum / 256;  // cual bloque indirecto usar
        int offset     = blockNum % 256;  // posicion dentro de ese bloque

        uint16_t indir_block[256];
        int sector = inp->i_addr[indir_index];
        if (sector == 0) return -1;

        if (diskimg_readsector(fs->dfd, sector, indir_block) != DISKIMG_SECTOR_SIZE)
            return -1;

        int target = indir_block[offset];
        return (target == 0) ? -1 : target;
    }

    // doble indirección
    blockNum -= 7 * 256;
    if (blockNum >= 256 * 256) return -1;  // fuera de rango

    // leemos el bloque de punteros indirectos
    uint16_t dbl_block[256];
    int dbl_sector = inp->i_addr[7];
    if (dbl_sector == 0) return -1;
    if (diskimg_readsector(fs->dfd, dbl_sector, dbl_block) != DISKIMG_SECTOR_SIZE)
        return -1;

    int i = blockNum / 256;
    int j = blockNum % 256;
    int indir_sector = dbl_block[i];
    if (indir_sector == 0) return -1;

    // leemos el bloque indirecto correspondiente
    uint16_t indir_block[256];
    if (diskimg_readsector(fs->dfd, indir_sector, indir_block) != DISKIMG_SECTOR_SIZE)
        return -1;

    int target = indir_block[j];
    return (target == 0) ? -1 : target;
}


int inode_getsize(struct inode *inp) {
    if (!inp) return -1; // puntero invalido
    return ((inp->i_size0 << 16) | inp->i_size1);
}

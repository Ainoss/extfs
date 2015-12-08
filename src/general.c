
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "ext2.h"
#include "ext2fs.h"
#include "general.h"


int get_super_block()
{
    if (lseek(g_fs_info.img_desc, 1024, SEEK_SET) == -1){
        perror("Seek error");
        return -1;
    }
    if (read(g_fs_info.img_desc, &g_fs_info.sb, sizeof(g_fs_info.sb)) == -1){
        perror("Read error");
        return -1;
    }

    if (g_fs_info.sb.s_magic != 0xEF53){
        LOGE("Magic number dismatch\n");
        return -1;
    }
    if (g_fs_info.sb.s_rev_level != 1){
        LOGE("Magic number dismatch\n");
        return -1;
    }

    return 0;
}

int fs_img_init(const char *path)
{
    g_fs_info.img_desc = open(path, O_RDONLY);
    if (g_fs_info.img_desc == -1){
        perror("Filesystem image");
        return -1;
    }
    return 0;
}

uint32_t get_block_size(struct ext2_super_block *sb)
{
    return 1 << (sb->s_log_block_size + 10);
}

void* read_block(uint32_t id)
{
    uint32_t bsize = get_block_size(&g_fs_info.sb);
    void *blk = malloc(bsize);

    if (lseek(g_fs_info.img_desc, id * bsize, SEEK_SET) == -1){
        perror("Seek error");
        return NULL;
    }
    if (read(g_fs_info.img_desc, blk, bsize) == -1){
        perror("Read error");
        return NULL;
    }
    return blk;
}

int release_block(void *blk)
{
    free(blk);
    return 0;
}

int get_bg_desc(uint32_t bg_id, struct ext2_group_desc *gd)
{
    void *blk;
    uint32_t bsize = get_block_size(&g_fs_info.sb);
    uint32_t blk_num;
    uint32_t blk_offset;
    blk_num = (bsize == 1024 ? 2 : 1) + bg_id * sizeof(*gd) / bsize;
    blk_offset = bg_id * sizeof(*gd) % bsize;

    if (!(blk = read_block(blk_num)))
        return 1;

    memcpy(gd, (char*)blk + blk_offset, sizeof(*gd));

    release_block(blk);
    return 0;
}



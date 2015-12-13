
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "ext2.h"
#include "general.h"


int get_inode(uint32_t id, struct ext2_inode *pinode)
{
    uint32_t bg_num = (id - 1) / g_fs_info.sb.s_inodes_per_group;
    uint32_t bg_offset = (id - 1) % g_fs_info.sb.s_inodes_per_group;
    uint32_t bsize = get_block_size(&g_fs_info.sb);
    void *blk;
    uint32_t blk_num, blk_offset;
    struct ext2_group_desc gd;

    if (get_bg_desc(bg_num, &gd))
        return 1;
    blk_num = gd.bg_inode_table + bg_offset * sizeof(*pinode) / bsize;
    blk_offset = bg_offset * sizeof(*pinode) % bsize;

    if (!(blk = read_block(blk_num)))
        return 1;

    memcpy(pinode, (char*)blk + blk_offset, sizeof(*pinode));

    release_block(blk);
    return 0;
}

uint32_t get_iblock(const struct ext2_inode *pinode, uint32_t iblock_num)
{
    const uint32_t bsize = get_block_size(&g_fs_info.sb);
    const uint32_t indirect_num = bsize / sizeof(pinode->i_block[0]);
    const uint32_t dbl_ind_num = indirect_num * indirect_num;
    const uint32_t direct_num = 12;

    if (iblock_num < direct_num)
        return pinode->i_block[iblock_num];
    else {
        void* blk;
        uint32_t res = 0;
        uint32_t interm_blk;
        if ((iblock_num -= direct_num) < indirect_num){
            interm_blk = pinode->i_block[direct_num];
        }
        else if ((iblock_num -= indirect_num) < dbl_ind_num){
            if (!(blk = read_block(pinode->i_block[direct_num + 1])))
                return 0;
            interm_blk = ((uint32_t*)blk)[iblock_num / indirect_num];
            release_block(blk);
            iblock_num %= indirect_num;
        }
        else if ((iblock_num -= dbl_ind_num) < dbl_ind_num * indirect_num){
            if (!(blk = read_block(pinode->i_block[direct_num + 2])))
                return 0;
            interm_blk = ((uint32_t*)blk)[iblock_num / dbl_ind_num];
            release_block(blk);
            iblock_num %= dbl_ind_num;

            if (!(blk = read_block(interm_blk)))
                return 0;
            interm_blk = ((uint32_t*)blk)[iblock_num / indirect_num];
            release_block(blk);
            iblock_num %= indirect_num;
        }
        else {
            LOGE("Too big iblock_num!\n");
            return 0;
        }

        if (!(blk = read_block(interm_blk)))
            return 0;
        res = ((uint32_t*)blk)[iblock_num];
        release_block(blk);
        return res;
    }
    return 0;
}

void* read_inode_block(const struct ext2_inode *pinode, uint32_t iblock_num)
{
    uint32_t blk_num;
    if (!(blk_num = get_iblock(pinode, iblock_num)))
        return NULL;
    return read_block(blk_num);
}


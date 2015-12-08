
#ifndef EXT2_GENERAL_H
#define EXT2_GENERAL_H

#include <stdio.h>
#include <inttypes.h>
#include "ext2.h"

#define LOGE(...) fprintf(stderr, __VA_ARGS__)
#define EXT2_NAME_LEN 255

enum {
    SP_ROOT_INODE = 2,
};

struct ext2fs_info {
    int img_desc;
    struct ext2_super_block sb;
} g_fs_info;

int get_super_block();
uint32_t get_block_size(struct ext2_super_block *sb);
void* read_block(uint32_t id);
int release_block(void *blk);
int get_bg_desc(uint32_t bg_id, struct ext2_group_desc *gd);
int get_inode(uint32_t id, struct ext2_inode *inode);
//uint32_t get_iblock(const struct ext2_inode *inode, uint32_t iblock_num);
void* read_inode_block(const struct ext2_inode *pinode, uint32_t iblock_num);
int dir_get_next_entry(struct ext2_dir_entry *de, uint32_t inode_id,
                       uint32_t *poffset, const char *name);
uint32_t find_path_inode(const char *path);


#endif  //EXT2_GENERAL_H

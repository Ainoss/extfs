
#ifndef EXT2_USER_H
#define EXT2_USER_H

#include <inttypes.h>
#include "ext2.h"

int fs_img_init(const char *path);
int get_super_block();
int get_inode(uint32_t id, struct ext2_inode *inode);

#endif //EXT2_USER_H


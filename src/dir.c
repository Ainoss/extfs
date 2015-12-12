
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "ext2.h"
#include "general.h"

int dir_get_next_block_entry(struct ext2_dir_entry *de, uint32_t *poffset, 
                             const void *blk)
{
    uint32_t bsize = get_block_size(&g_fs_info.sb);
    do {
        memcpy(de, (char*)blk + *poffset, sizeof(*de));
        if (de->inode != 0){
            memcpy(de->name, (char*)blk + *poffset + sizeof(*de), de->name_len);
            *poffset += de->rec_len;
            return 0;
        }
        *poffset += de->rec_len;
    } while (*poffset != bsize);
    
    return 1;
}

int dir_get_next_entry(struct ext2_dir_entry *de, uint32_t inode_id, 
                       uint32_t *poffset, const char *name)
{
    uint32_t bsize = get_block_size(&g_fs_info.sb);
    uint32_t offset = *poffset % bsize;
    uint32_t iblock_num = *poffset / bsize;
    struct ext2_inode inode;
    void *blk = NULL;

    get_inode(inode_id, &inode);
    while (iblock_num < inode.i_size / bsize) {
        if (!(blk = read_inode_block(&inode, iblock_num))){
            return -1;
        }
        do {
            if (!dir_get_next_block_entry(de, &offset, blk)){
                if (name == NULL || (strlen(name) == de->name_len 
                    && !strncmp(de->name, name, de->name_len))) 
                {
                    release_block(blk);
                    *poffset = iblock_num * bsize + offset;
                    return 0;
                }
            }
        } while (offset != bsize);
        iblock_num++;
        offset = 0;
    }

    *poffset = inode.i_size;
    memset(de, 0, sizeof(*de));
    if (blk)
        release_block(blk);
    return 0;
}

uint32_t dir_find_entry_inode(uint32_t dir_inode_id, const char *name)
{
    uint32_t res, offset = 0;
    struct ext2_dir_entry *de = malloc(sizeof(*de) + EXT2_NAME_LEN);
    if (!de)
        return 0;
    if (dir_get_next_entry(de, dir_inode_id, &offset, name) == -1){
        free(de);
        return 0;
    }
    res = de->inode;
    free(de);
    return res;
}

uint32_t find_path_inode(const char *path)
{
    uint32_t inode_id = SP_ROOT_INODE;
    char *name, del[] = "/";
    char *vpath = strdup(path);
    if (vpath == NULL){
        LOGE("Lack of memory!\n");
        return 0;
    }
    if (path[0] != '/'){
        LOGE("Only absolute path is supported!\n");
        free(vpath);
        return 0;
    }

    name = strtok(vpath, del);
    do {
        if (!(inode_id = dir_find_entry_inode(inode_id, name))) {
            LOGE("Invalid path!\n");
            free(vpath);
            return 0;
        }
    } while ((name = strtok(NULL, del)) != NULL);

    free(vpath);
    return inode_id;
}



#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include "ext2.h"
#include "general.h"
#include "ext2fs_api.h"

#define MAX_FILE_DESC 16

struct file_desc_internal {
    uint32_t inode;
    uint64_t position;
} fdis[MAX_FILE_DESC];

DIR_EXT2 dires[MAX_FILE_DESC];

int open_ext2(const char *pathname)
{
    unsigned i;
    for (i = 0; i < MAX_FILE_DESC; i++)
        if (fdis[i].inode == 0){
            fdis[i].inode = find_path_inode(pathname);
            if (fdis[i].inode == 0)
                return -1;
            fdis[i].position = 0;
            return i;
        }
    return -1;
}

int close_ext2(int fd)
{
    fdis[fd].inode = 0;
    return 0;
}

ssize_t read_ext2(int fd, void *buf, size_t count)
{
    uint32_t bsize = get_block_size(&g_fs_info.sb);
    uint64_t position = fdis[fd].position;
    uint32_t blk_num, blk_offset, blk_count;
    size_t res = count;
    struct ext2_inode inode;
    void* blk;

    if (get_inode(fdis[fd].inode, &inode))
        return -1;

    while (count){
        blk_num = position / bsize;
        blk_offset = position % bsize;
        blk_count = bsize - blk_offset;
        if (blk_count > count) 
            blk_count = count;

        if (!(blk = read_inode_block(&inode, blk_num)))
            return -1;
        memcpy(buf, (char*)blk + blk_offset, blk_count);
        buf += blk_count;
        position += blk_count;
        count -= blk_count;
        release_block(blk);
    }
    lseek_ext2(fd, res, SEEK_CUR);
    return res;
}

off_t lseek_ext2(int fd, off_t offset, int whence)
{
    if (whence == SEEK_SET)
        fdis[fd].position = offset;
    else if (whence == SEEK_CUR)
        fdis[fd].position += offset;
    else if (whence == SEEK_END){
        struct ext2_inode inode;
        if (get_inode(fdis[fd].inode, &inode))
            return -1;
        fdis[fd].position = inode.i_size + offset;
    }
    return fdis[fd].position;
}

int fstat_ext2(int fd, struct stat_ext2 *buf)
{
    struct ext2_inode inode;
    if (get_inode(fdis[fd].inode, &inode))
        return -1;
    buf->st_ino = fdis[fd].inode;
    buf->st_size = inode.i_size;
    buf->st_mode = inode.i_mode;
    return 0;
}

DIR_EXT2 *opendir_ext2(const char *name)
{
    unsigned i;
    for (i = 0; i < MAX_FILE_DESC; i++)
        if (dires[i].inode == 0){
            dires[i].inode = find_path_inode(name);
            struct ext2_inode inode;
            if (dires[i].inode == 0)
                return NULL;
            if (get_inode(dires[i].inode, &inode))
                return NULL;
            if (!(inode.i_mode & S_IFDIR)){
                LOGE("This is not directory!\n");
                return NULL;
            }
            dires[i].offset = 0;
            return &dires[i];
        }
    return NULL;
}

int closedir_ext2(DIR_EXT2 *dirp)
{
    dirp->inode = 0;
    return 0;
}

int readdir_r_ext2(DIR_EXT2 *dirp, struct dirent *entry, struct dirent **result)
{
    struct ext2_dir_entry *de = alloca(sizeof(*de) + EXT2_NAME_LEN);
    if (!de)
        return 0;

    if (dir_get_next_entry(de, dirp->inode, &dirp->offset, NULL) == -1){
        LOGE("Error getting next entry!\n");
        return 1;
    }
    if (de->inode == 0){
        *result = NULL;
        return 0;
    }
    memset(entry, 0, sizeof(*entry));
    entry->d_ino = de->inode;
    memcpy(entry->d_name, de->name, de->name_len);
    entry->d_name[de->name_len] = 0;
    *result = entry;
    return 0;
}


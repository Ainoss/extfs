
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include "ext2.h"
#include "general.h"

struct stat_ext2 {
    ino_t  st_ino;
    off_t  st_size;
    mode_t st_mode;
};

typedef struct dir_ext_t {
    uint32_t inode;
    uint32_t offset;
} DIR_EXT2;

int fs_img_init(const char *path);
int get_super_block();

int open_ext2(const char *pathname);
int close_ext2(int fd);
ssize_t read_ext2(int fd, void *buf, size_t count);
off_t lseek_ext2(int fd, off_t offset, int whence);
int fstat_ext2(int fd, struct stat_ext2 *buf);

DIR_EXT2 *opendir_ext2(const char *name);
int closedir_ext2(DIR_EXT2 *dirp);
int readdir_r_ext2(DIR_EXT2 *dirp, struct dirent *entry, struct dirent **result);


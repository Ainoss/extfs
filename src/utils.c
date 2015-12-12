
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ext2.h"
#include "ext2fs_api.h"
#include "utils.h"


int get_full_path(const char *start, const char *end, char *full_path)
{
    size_t len;
    if (end[0] == '/'){
        strcpy(full_path, end);
        return 0;
    }
    if (!strncmp(end, "./", 2))
        end += 2;
    strcpy(full_path, start);
    len = strlen(full_path);
    if (full_path[len-1] != '/'){
        full_path[len] = '/';
        full_path[len + 1] = 0;
    }
    strncat(full_path, end, 1024);
    return 0;
}

int exec_ls(const char *arg, const char *pwd)
{
    struct dirent dentry, *res_dentry;
    char path[1024];
    DIR_EXT2 *pdir;

    if (!arg)
        strcpy(path, pwd);
    else
        get_full_path(pwd, arg, path);
    pdir = opendir_ext2(path);
    if (!pdir){
        LOGE("Failed opendir\n");
        return 1;
    }

    while (1) {
        if (readdir_r_ext2(pdir, &dentry, &res_dentry)){
            LOGE("Error reading next dirent\n");
            closedir_ext2(pdir);
            return 1;
        }
        if (!res_dentry)
            break;
        printf("%8u\t%s\n", 0, dentry.d_name);
    }

    closedir_ext2(pdir);
    return 0;
}

int exec_cat(const char *arg, const char *pwd)
{
    int fd;
    char str[1024];
    char path[1024];
    struct stat_ext2 st;
    uint64_t offset = 0;

    if (!arg){
        LOGE("Usage: cat file");
        return 1;
    }
    get_full_path(pwd, arg, path);
    if ((fd = open_ext2(path)) == -1){
        LOGE("Error open\n");
        return 1;
    }
    if (fstat_ext2(fd, &st)){
        LOGE("Error fstat\n");
        return 1;
    }

    while (offset < st.st_size) {
        uint32_t size = st.st_size - offset;
        if (size > sizeof(str))
            size = sizeof(str);
        if (read_ext2(fd, str, size) == -1){
            LOGE("Error read\n");
            return 1;
        }
        fwrite(str, 1, size, stdout);
        offset += size;
    }

    close_ext2(fd);
    return 0;
}

int exec_cd(const char *arg, char *pwd)
{
    char path[1024];
    DIR_EXT2 *pdir;

    get_full_path(pwd, arg, path);
    pdir = opendir_ext2(path);
    if (!pdir){
        LOGE("Can't change directory to %s\n", path);
        return 1;
    }
    closedir_ext2(pdir);
    
    strcpy(pwd, path);
    return 0;
}


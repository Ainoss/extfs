
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "ext2.h"
#include "ext2fs_api.h"
#include "utils.h"


char g_pwd[1024] = "/";


int main_loop()
{
    char str[1024], *str2, *cmd, *arg;
    int stop = 0, res;

    while (!stop){
        printf("%s> ", g_pwd);
        errno = 0;
        if (!fgets(str, sizeof(str), stdin)){
            if (errno){
                perror("fgets");
                return 1;
            }
            break;
        }
        str2 = strdup(str);
        cmd = strtok(str2, " \n");
        arg = strtok(NULL, " \n");
        if (!strcmp(cmd, "ls")){
            if ((res = exec_ls(arg, g_pwd)) != 0)
                printf("Error code: %u\n", res);
        }
        else if (!strcmp(cmd, "cat")){
            if ((res = exec_cat(arg, g_pwd)) != 0)
                printf("Error code: %u\n", res);
        }
        else if (!strcmp(cmd, "cd")){
            if ((res = exec_cd(arg, g_pwd)) != 0)
                printf("Error code: %u\n", res);
        }
        else if (!strcmp(cmd, "quit"))
            stop = 1;
        free(str2);
    }

    return 0;
}

int main()
{
    const char fs_img_path[] = "./fs.img";
    fs_img_init(fs_img_path);
    get_super_block();

    main_loop();

    return 0;
}


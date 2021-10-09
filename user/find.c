#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"


char *strcat(char *s1, const char *s2) {
    char *b = s1;

    while (*s1) ++s1;
    while (*s2) *s1++ = *s2++;
    *s1 = 0;

    return b;
}

char *fmtname(char *path) {
   char *p;
    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
      ;
    p++;
    return p;
}


void find(char *path, char *target) {
    char buf[512], *p;
    int fd;
    struct stat st;
    struct dirent de;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    if (st.type != T_DIR) {
        fprintf(2, "find: not dir %s\n", path);
        return;
    }

    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
        printf("find: path too long\n");
        return;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (stat(buf, &st) < 0) {
            printf("find: cannot stat %s\n", buf);
            continue;
        }
        char *fileName = fmtname(buf);
        if (strcmp(fileName, ".")==0 || strcmp(fileName, "..")==0){
            continue;
        }
        char tmp[512];
        strcpy(tmp, path);
        char* newPath = strcat(strcat(tmp,"/"),fileName);
         if (strcmp(fileName, target) ==0)
                    {
                        printf("%s\n",newPath);
                    }
        if (st.type == T_DIR) {
            find(newPath, target);
        }
    }
    close(fd);
}


int
main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "error\n");
        exit(0);
    }
    find(argv[1], argv[2]);
    exit(0);
}




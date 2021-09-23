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

char *
fmtname(char *path) {
    static char buf[DIRSIZ + 1];
    char *p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--);
    p++;

    // Return blank-padded name.
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
    return buf;
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
    printf("file type %d\n",st.type);
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
         printf("file name : %s\n",fileName);
         printf("%d,%d\n",strcmp(fileName, ".") ==0, strcmp(fileName, "..")==0);
        if (strcmp(fileName, ".") || strcmp(fileName, "..")){
            continue;
        }
        char* newPath = strcat(strcat(path,"/"),fileName);
        printf("new pth :%s\n",newPath);
        if (st.type == T_DIR) {
            find(newPath, target);
            continue;
        }
        if (st.type == T_FILE) {
            if (strcmp(fileName, target))
            {
                printf("%s\n",newPath);
            }
            continue;
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
     printf("path :%s, target:%s\n",argv[1],argv[2]);
    find(argv[1], argv[2]);
    exit(0);
}




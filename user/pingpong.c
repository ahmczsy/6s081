#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p[2];
  char child_buf[0];
  char parent_buf[0];
  pipe(p);
  if (fork() ==0) {
    read(p[0],child_buf,1);
    fprintf(2,"%d: received ping\n",getpid());
    write(p[1],"8",1);

    exit(0);
  } else {
    write(p[1],"9",1);
    wait(0);
    read(p[0],parent_buf,1);
    fprintf(2,"%d: received pong\n",getpid());

    exit(0);
  }

}



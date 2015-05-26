#ifndef __SOCK_TOOLS_H__
#define __SOCK_TOOLS_H__

#include <sys/socket.h>
#include <errno.h>

void err_log(const char * msg) {
  printf("error id %d %s\n", errno, msg);
}



int sock_init(int port) {
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(0);

  err_log("123");
  

  int sfd = socket(AF_INET, SOCK_STREAM, 0);

  int on = 1;
  setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  if(bind(sfd, (sockaddr *)&addr, sizeof(addr))) {
    err_log("bind error");
    return -1;
  }

  if(listen(sfd, 10)) {
    err_log("listen_error");
    return -1;
  }
  return sfd;
}

char rot_13(char ch) {
  if (( 'A' <= ch && ch <= 'M') || ('a' <= ch && ch <= 'm')) {
    return (ch + 13);
  } else if (( 'N' <= ch && ch <= 'Z') || ( 'n' <= ch && ch <= 'z')) {
    return (ch - 13);
  } else {
    return ch;
  }
}

#endif // __SOCK_TOOLS_H__

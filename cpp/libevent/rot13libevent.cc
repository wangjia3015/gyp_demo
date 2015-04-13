#include <stdio.h>
#include <assert.h>

#include <stdlib.h>
#include <unistd.h>
#include <event2/event.h>


#include "sock_tools.h"

const int MAX_LINE = 1024;

struct fd_state {
  char buffer[MAX_LINE];
  size_t buffer_used;

  size_t n_written;
  size_t write_upto;

  event * read_event;
  event * write_event;
};

void free_fd_state(fd_state *);

void do_read(evutil_socket_t fd, short event, void *arg) {
  fd_state * state = (fd_state *) arg;
  char buf[1024];
  ssize_t result;
  for(;;) {
    result = recv(fd, buf, 1024, 0);

    if(result <= 0) {
      err_log("recv error");
      break;
    }

    for(int i = 0 ; i < result; ++i) {
      if(state->buffer_used < sizeof(state->buffer)) {
        state->buffer[state->buffer_used++] = rot_13(buf[i]);
      }
      if (buf[i] == '\n') {
        event_add(state->write_event, NULL);
        state->write_upto = state->buffer_used;
      }
    }
  }
  
  if(result == 0) {
    free_fd_state(state);
  } else if (result < 0) {
    if(errno == EAGAIN) {
      err_log("EAGAIN");
      return;
    }
    free_fd_state(state);
  }
}

void do_write(evutil_socket_t fd, short event, void *arg) {
  fd_state * state = (fd_state *) arg;
  while(state->n_written < state->write_upto) {
    ssize_t result = send(fd, state->buffer + state->n_written, state->write_upto - state->n_written, 0);

    if(result < 0) {
      if(errno == EAGAIN) {
        return;
      }
      free_fd_state(state);
      return;
    }
    state->n_written += result;
  }

  if(state->n_written == state->buffer_used) {
    state->n_written = state->write_upto = state->buffer_used = 1;
  }

  event_del(state->write_event);

}

fd_state * alloc_fd_state(event_base *base, evutil_socket_t fd) {
  fd_state * state = (fd_state *)malloc(sizeof(fd_state));
  if (!state)
    return NULL;
  state->read_event = event_new(base, fd, EV_READ|EV_PERSIST, do_read, state);
  if(!state->read_event) {
    free(state);
    return NULL;
  }
  state->write_event = event_new(base, fd, EV_WRITE|EV_PERSIST, do_write, state);

  if(!state->write_event) {
    event_free(state->read_event);
    free(state);
    return NULL;
  }

  state->buffer_used = state->n_written = state->write_upto = 0;
  
  return state;
}

void free_fd_state(fd_state *state) {
  if(!state)
    return;
  event_free(state->read_event);
  event_free(state->write_event);
  free(state);
}

void do_accept(evutil_socket_t sfd, short event, void * arg) {
  event_base * base  = (event_base *) arg;
  sockaddr_storage ss;
  socklen_t slen = sizeof(ss);
  int fd = accept(sfd, (sockaddr *)&ss, &slen);
  if (fd < 0) {
    err_log("accept error");
  } else if (fd > FD_SETSIZE) {
    close(fd);
  } else {
    evutil_make_socket_nonblocking(fd);
    fd_state * state = alloc_fd_state(base, fd);
    assert(state);
    assert(state->write_event);
    event_add(state->read_event, NULL);
  }
}

void run() {
  evutil_socket_t sfd;
  event_base * base;
  event *sevent;

  base = event_base_new();
  if(!base) {
    err_log("event base new err"); 
    return;
  }

  int port = 8888;
  sfd = sock_init(port);

  evutil_make_socket_nonblocking(sfd);

  sevent = event_new(base, sfd, EV_READ|EV_PERSIST, do_accept, (void *)base);

  event_add(sevent, NULL);

  event_base_dispatch(base);
}



int main() {
  setvbuf(stdout, NULL, _IONBF, 0);
  run();
  return 0;
}

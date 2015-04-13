#include <stdio.h>

#include "../socket/sockettools.h"

struct child {
	pid_t child_pid;
	int child_pipefd;
	int child_status;
	long child_count;
}

int main() {
	const int sub_process_count = 5;

	fd_set set;
	int port = 8888;
	int sfd = socket_tcp_server(port);

	if(sfd < 0) {
		err_log("accept");
		return 0;
	}

	FD_ZERO(&set);
	FD_SET(sfd, &set);
	int maxfd = sfd;
	child **ptr = (child **)calloc(sub_process_count, sizeof(child));
	
	for (int i = 0 ; i < sub_process_count, ++i) {
		child_make();
		FD_SET(,);
	}

}

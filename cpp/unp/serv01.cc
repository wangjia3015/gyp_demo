#include <stdio.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "../socket/sockettools.h"


void sig_chld(int sig) {
	int pid;
	int stat;
	if((pid = waitpid(-1, &stat, WNOHANG) > 0)) {
		printf("progress terminated id %d\n", pid);
	}
}

void sig_int(int sig) {
	pr_cpu_time();
	exit(0);
}

int main(int argc, char ** argv) {
	
	if(argc != 2) {
		log("argc error svr01 port");
		return 0;
	}
	
	int port = atoi(argv[1]);

	int sfd = socket_tcp_server(port);

	signal(SIGCHLD, sig_chld);
	signal(SIGINT, sig_int);

	int cfd; 
	for(;;) {
		if((cfd = accept(sfd, NULL, NULL)) < 0) {
			if (errno == EINTR) {
				continue;
			}
			err_log("accept error");
			return 1;
		}

		if (fork() == 0) {
			close(sfd);
			//web_child(cfd);
			sock_deal_client(cfd);
			exit(0);
		}
		close(cfd);
	}

	return 0;
}

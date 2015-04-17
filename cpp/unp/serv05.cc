#include <stdio.h>

#include "../socket/sockettools.h"
#include "trans_fd.h"
#define LOG(msg) err_log(msg);

struct child {
	pid_t child_pid;
	int child_pipefd;
	int child_status;
	long child_count;
};

void child_work(int pipefd) {
	// wait fd
	// read fd
	// do work
	// write 1 byte back
	int cfd;
	for (;;) {
		printf("pipeid %d\n", pipefd);
		if ((cfd = read_fd(pipefd)) > 0) {
			printf("cfd %d errno %d\n", cfd, errno);
			if(sock_deal_client(cfd) <= 0) {
				err_log("sock_deal_client error");
			}
			close(cfd);
		}
		int n = 1;
		int s = sizeof(n);
		if(writen(pipefd, &n, s) != s) {
			err_log("client write err");
			exit(0);
		}
	}
}

void child_make(child *ptr, int i, int sfd) {
	// 
	int fds[2];
	if(socketpair(AF_UNIX, SOCK_STREAM, 0, fds) != 0) {
		err_log("create sub_process error");
		exit(0);
	}

	ptr[i].child_pipefd = fds[0];
	ptr[i].child_status = 0;
	ptr[i].child_count = 0;

	int pid; 
	if ((pid = fork()) == 0) { // children
		close(fds[0]);
		child_work(fds[1]);
	} else {
		ptr[i].child_pid = pid;
		close(fds[1]);
	}
}

int main() {
	const int sub_process_count = 5;
	ssize_t addrlen;
	fd_set set;
	fd_set masterset;

	int port = 8888;
	int sfd = socket_tcp_server(port);

	if(sfd < 0) {
		err_log("accept");
		return 0;
	}

	FD_ZERO(&set);
	FD_SET(sfd, &set);
	int maxfd = sfd;
	child *ptr = (child *)calloc(sub_process_count, sizeof(child));
	
	for (int i = 0 ; i < sub_process_count; ++i) {
		child_make(ptr, i, sfd); // create and start
		FD_SET(ptr[i].child_pipefd, &set);
		maxfd = std::max( ptr[i].child_pipefd, maxfd);
	}

	int navail = sub_process_count;
	int nsel;
	for(;;) {
		masterset = set;
		if(navail <= 0) {
			FD_CLR(sfd, &masterset);
		}
		nsel = select(maxfd + 1, &masterset, NULL, NULL, NULL);

		if (nsel <= 0) {
			err_log("accept error");
			exit(0);
		}
		
		if(FD_ISSET(sfd, &masterset)) {
			//
			int cfd = accept(sfd, NULL, NULL);
			printf("accept cfd %d\n", cfd);
			
			int i = 0;
			for(;i < sub_process_count; ++i) {
				if(ptr[i].child_status == 0)
					break;
			}
			
			if(i == sub_process_count) {
				// first coding error
				err_log("err busy sub process count");
				exit(0);
			}
			
			// TODO write fd and wait result
			if (write_fd(ptr[i].child_pipefd, cfd) > 0) {
				ptr[i].child_status = 1;
				ptr[i].child_count++;
				navail--;
			}
			close(cfd);

			if(--nsel == 0) {
				continue;
			}
		}

		for(int i = 0 ; i < sub_process_count; ++i) {
			if(FD_ISSET(ptr[i].child_pipefd, &masterset)) {
				int cfd = ptr[i].child_pipefd;
				int n;
				int ret = read(cfd, &n, sizeof(n));
				if (ret <= 0) {
					// TODO restart sub process
					// now always in use
					//error
					char buf[128];
					snprintf(buf, 128, "sub_process %d read error", i);
					err_log(buf);
				}
				ptr[i].child_status = 0;
				navail++;
				if(--nsel==0) {
					break;
				}
			}
		} 

	}

}

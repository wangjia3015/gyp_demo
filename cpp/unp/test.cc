#include <stdio.h>
#include "trans_fd.h"

#include "../socket/sockettools.h"

//int write_fd(int sfd, int cfd) {
//	if(send(sfd, &cfd, sizeof(cfd), 0) <= 0) {
//		err_log("send msg error");
//		return -1;
//	}
//	return 1;
//}
//
//int read_fd(int sfd) {
//	int cfd;
//	if(recv(sfd, &cfd, sizeof(cfd), 0) <= 0) {
//		err_log("recv msg error");
//		return -1;
//	}
//	return cfd;
//}

int main() {
	int sfd = socket_tcp_server(8888);
	int cfd;
	int fds[2];

	if(socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0) {
		err_log("socket pair error");
		return 0;
	}

	if(fork() == 0) {
		log("enter sub process");
		if ((cfd = read_fd(fds[1])) < 0) {
			err_log("read fd error");
			return 0;
		} else {
			log("read_fd end...");
			printf("fd %d", cfd);
			sock_deal_client(cfd);
			close(cfd);
		}
	} else {
		if ((cfd = accept(sfd, NULL, NULL)) < 0) {
			err_log("accept error");
			return 0;
		}
		if (write_fd(fds[0], cfd) < 0) {
			err_log("write fd error");
			return 0;
		} else {
			log("write fd done");
		}
		close(cfd);
//		sleep(1024);
	}

	return 0;
}

#ifndef __SOCKET_TOOLS_H__
#define __SOCKET_TOOLS_H__

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>

#include <set>

void err_log(const char * msg) {
	printf("error : %s err code %d\n", msg, errno);
}

void log(const char * msg) {
	printf("log : %s \n", msg);
}


int socket_tcp_server(int port) {

	int sfd = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in addr;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sfd, (sockaddr *) &addr, sizeof(addr))) {
		err_log("bind error");
		return -1;
	}

	if(listen(sfd, 5)) {
		err_log("listen error");
		return -1;
	}
	
	return sfd;
}

void set_socket_reuse(int fd) {
	const int on = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
}

// ----------------------------------------------
void sock_send_msg(int fd) {
	const char * buf = "hello world";
	send(fd, buf, sizeof(buf), 0);
}

// ----------------------------------------------



/*
 *	现象 第一个timeout 之前没问题 timeout 之后问题就来了
 * */
void sock_select_mode(int sfd) {
	fd_set tset;
	fd_set set;
	FD_ZERO(&tset);
	FD_SET(sfd, &tset);
//	std::set<int> active_fds;

	for(;;) {
		FD_ZERO(&set);
		set = tset;
		timeval timeout = {1, 0};
		int ret = select(sfd+1, &set, NULL, NULL, &timeout);
		if(ret < 0) {
			err_log("select error");
			break;
		} else if (ret == 0) {
			printf("select timeout...\n");
		} else {
			if (FD_ISSET(sfd, &set)) {
				log("get accept...");
				int cfd = accept(sfd, NULL, NULL);
				sock_send_msg(cfd);
				active_fds.push_back(cfd);
				FD_SET(cfd, &tset);
				close(cfd);
			}

			for (std::set<int>::iterator it = active_fds.begini(); it != active_fds.end(); ++it) {
				if(FD_ISSET(*it, &set)) {
					// client active
					// read bytes and write back
					// if error return
					int cfd = *it;
					const int MAX_NUM = 1024;
					char buf[MAX_NUM];
					int num = 0;
					// TODO 退出机制如果返回0 代表什么???
					if((num = recv(cfd, buf, MAX_NUM, 0)) < 0 ) {
						// clear
						active_fds.erase(cfd);
						FD_CLR(cfd, &tset);
						close(cfd);
					}
				}
			}
		}
	}

}


#endif // __SOCKET_TOOLS_H__


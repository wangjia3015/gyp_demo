#ifndef __SOCKET_TOOLS_H__
#define __SOCKET_TOOLS_H__

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/resource.h>

#include <errno.h>
#include <arpa/inet.h>

#include <fcntl.h>

#include <string.h>

#include <algorithm>
#include <set>
void set_socket_reuse(int);

void err_log(const char * msg) {
	printf("error : %s err code %d\n", msg, errno);
}

void log(const char * msg) {
	printf("log : %s \n", msg);
}

void make_nonblocking(int fd) {
	fcntl(fd, F_SETFL, O_NONBLOCK);
}

void pr_cpu_time() {
	double			user, sys;
	struct rusage	myusage, childusage;

	if (getrusage(RUSAGE_SELF, &myusage) < 0)
		err_log("getrusage error");
	if (getrusage(RUSAGE_CHILDREN, &childusage) < 0)
		err_log("getrusage error");

	user = (double) myusage.ru_utime.tv_sec +
									myusage.ru_utime.tv_usec/1000000.0;
	user += (double) childusage.ru_utime.tv_sec +
									childusage.ru_utime.tv_usec/1000000.0;
	sys = (double) myusage.ru_stime.tv_sec +
								 myusage.ru_stime.tv_usec/1000000.0;
	sys += (double) childusage.ru_stime.tv_sec +
								  childusage.ru_stime.tv_usec/1000000.0;

	printf("\nuser time = %g, sys time = %g\n", user, sys);
}

int socket_tcp_server(int port) {

	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("sfd %d\n", sfd);

	set_socket_reuse(sfd);

	sockaddr_in addr;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sfd, (sockaddr *) &addr, sizeof(addr))) {
		err_log("bind error");
		return -1;
	}

	if(listen(sfd, 10000)) {
		err_log("listen error");
		return -1;
	}
	
	return sfd;
}

void sock_set_nonblocking(int fd) {
  fcntl(fd, F_SETFL, O_NONBLOCK);  
}

void set_socket_reuse(int fd) {
	int on = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on))) {
		err_log("setsockopt error");
	}
}

// ----------------------------------------------
void sock_send_msg(int fd) {
	const char * buf = "hello world\n";
	send(fd, buf, strlen(buf), 0);
}

int max_fd(const std::set<int> &set) {
	std::set<int>::const_iterator cit = std::max_element(set.begin(), set.end());
	if (cit != set.end()) {
		return (*cit);
	}
	return -1;
}

int sock_deal_client(int cfd) {
	const int MAX_NUM = 1024;
	char buf[MAX_NUM];
	int num = 0;
	// TODO 退出机制如果返回0 代表什么???
	if((num = recv(cfd, buf, MAX_NUM, 0)) <= 0 ) {
		// clear
		return num;
	}

	{
		buf[num] = 0;
		char sbuf[1024];
		sprintf(sbuf, "client %d send msg : %s", cfd, buf);
		log(sbuf);
	}
	
	if ((num = send(cfd, buf, num, 0)) <= 0) {
		return -1;
	}

	return cfd;
}

void set_nonblocking(int fd) {
  fcntl(fd, F_SETFL, O_NONBLOCK);
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
	std::set<int> active_fds;
	active_fds.insert(sfd);

	for(;;) {
		FD_ZERO(&set);
		set = tset;
		timeval timeout = {10, 0};
		int ret = select(max_fd(active_fds) + 1, &set, NULL, NULL, &timeout);
		if(ret < 0) {
			err_log("select error");
			break;
		} else if (ret == 0) {
			log("select timeout...");
		} else {
			if (FD_ISSET(sfd, &set)) {
				log("get accept...");
				int cfd = accept(sfd, NULL, NULL);
        set_nonblocking(cfd);
				sock_send_msg(cfd);
				active_fds.insert(cfd);
				FD_SET(cfd, &tset);
			}

			for (std::set<int>::iterator it = active_fds.begin(); it != active_fds.end(); ++it) {

				if((*it) == sfd)
					continue;

				if(FD_ISSET(*it, &set)) {
					// client active
					// read bytes and write back
					// if error return
					int cfd = *it;
					
					if (sock_deal_client(cfd) <= 0) {
						active_fds.erase(cfd);
						FD_CLR(cfd, &tset);
						close(cfd);
					}

				}
			}
		}
	}

}

//void sock_eopll_mode(int sfd) {
//  epoll_event ev;
//  ev.data.fd = sfd;
//  ev.events = EPOLLIN;
//
//  int epfd = epoll_create(1024);
//
//  int sfd = socket_tcp_server(8888);
//  
//  if(epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &ev) == -1) {
//    // TODO error
//    return ;
//  }
//  
//}

void sock_epoll_mode(int sfd) {
	const int max_event_count = 1024;
	epoll_event ev;
	epoll_event events[max_event_count];

	int epfd = epoll_create(max_event_count);
	
	if (epfd < 0) {
		err_log("epoll_create error");
		return;
	}

	ev.events = EPOLLIN;
	ev.data.fd = sfd;

	if (epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &ev)) {
		err_log("epoll_ctl error");
		return;
	}

	for (;;) {
		int fd_num = epoll_wait(epfd, events, max_event_count, -1);
		
		if(fd_num == -1) {
			err_log("epoll_wait error");
		}

		for(int index = 0 ; index < fd_num ; ++index) {
			if (events[index].data.fd == sfd) {
				// accept new socket
				int cfd = accept(sfd, NULL, NULL);
				ev.events = EPOLLIN | EPOLLET; // TODO
				ev.data.fd = cfd;

//				setnonblocking(cfd);

				if(epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev)) {
					err_log("accept epoll_ctl error");
					close(cfd);
				}
			} else {
				// other message come
				int fd = events[index].data.fd;
				sock_deal_client(fd);
				epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
				close(fd);
			}
		}
	}
}

int writen(int fd, void * sbuf, size_t buf_size) {
	char * buf = (char *)sbuf;
	int ret;
	int left_size = buf_size;
	while(left_size > 0) {
		ret = write(fd, buf, left_size);
		if(ret <= 0) {
			err_log("write err");
			return (buf_size - left_size);
		} 

		left_size -= ret;
		buf += ret;
	}
	return buf_size;
}

int readn(int fd, void * sbuf, size_t buf_size) {
	char * buf = (char *)sbuf;
	int ret;
	int s = buf_size;
	while(s > 0) {
		ret = read(fd, buf, s);
		if(ret <= 0) {
			err_log("read err");
			return (buf_size - s);
		}

		s -= ret;
		buf += ret;
	}
	return buf_size;
}

//int read_fd(int fromfd) {
//	int nfd;
//	int trans_size = sizeof(nfd);
//	if(readn(fromfd, &nfd, trans_size) != trans_size) {
//		return -1;
//	}
//	//nfd = ntohl(nfd);
//	return nfd;
//}
//
//int write_fd(int tofd, int cfd) {
////int nfd = htonl(cfd);
//	int nfd = cfd;
//	int trans_size = sizeof(nfd);
//	if(writen(tofd, &nfd, trans_size) != trans_size) {
//		return -1;
//	}
//	return trans_size;
//}

#endif // __SOCKET_TOOLS_H__


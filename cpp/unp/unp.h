#ifndef __UNP_H__
#define __UNP_H__

#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "../socket/sockettools.h"

#define MAXLINE 4096

int readn(int fd, char * buf, size_t len) {
	int cnt;
	int rc;

	cnt = len;

	while (cnt > 0) {
		rc = recv(fd, buf, cnt, 0);
		if (rc < 0) {
			if (errno == EINTR) {
				continue;
			}
			return -1;
		} else if (rc == 0) {
			return len - cnt;
		}
		
		buf += rc;
		cnt -= rc;
	}
	return len;
}

int tcp_connect(const char * host, const char * serv) {
	int fd , n;
	addrinfo hints, *res, * ressave;
	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
		err_log("getaddrinfo");
		return -1;
	} 

	ressave = res;

	do {
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(fd < 0) {
			err_log("socket connect error");
			continue;
		}
		
		if (connect(fd, res->ai_addr, res->ai_addrlen) == 0) {
			break;
		}

		close(fd);
	} while ((res = res->ai_next) != NULL);

	if (res == NULL) {
		err_log("tcp connect error");
		return -1;
	}

	freeaddrinfo(ressave);

	return fd;
}

#endif // __UNP_H__

#ifndef __TRANS_FD_H__
#define __TRANS_FD_H__

#include <sys/types.h>
#include <sys/socket.h>

// malloc
#include <stdlib.h>

#include <string.h>

#include "../socket/sockettools.h"

// transfer fd to 
int write_fd(int sfd, int cfd) {
	msghdr msg;
	iovec iov[1];
	char buf[1];
	iov[0].iov_base = buf;  // 这个东西没什么用，但是不加的话就发送不出去
	iov[0].iov_len = 1;
	const int cmsg_len = CMSG_LEN(sizeof(int));
	cmsghdr *cmptr = (cmsghdr *)malloc(cmsg_len);

	*(int *)CMSG_DATA(cmptr) = cfd;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	cmptr->cmsg_len = cmsg_len;
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;

	msg.msg_control = cmptr;
	msg.msg_controllen = cmsg_len;

	if(sendmsg(sfd, &msg, 0) < 0) {
		err_log("send msg error");
		return -1;
	}

	printf("send %d -> cfd %d\n", sfd, cfd);

	free(cmptr);
	return cfd;
}

int read_fd(int sfd) {
	msghdr msg;
	//memset(&msg, 0, sizeof(msg));
	iovec iov[1];
	char buf[1];
	iov[0].iov_base = buf;
	iov[0].iov_len = 1;

	const int cmsg_len = CMSG_LEN(sizeof(int));
	cmsghdr *cmptr = (cmsghdr *)malloc(cmsg_len);
	
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_control = cmptr;
	msg.msg_controllen = cmsg_len;

	printf("before get cfd cmsg_len %d\n", cmsg_len);
	if(recvmsg(sfd, &msg, 0) < 0) {
		err_log("recvmsg error");
		return -1;
	}

	int cfd = *(int *)CMSG_DATA(cmptr);

	printf("get cfd %d\n", cfd);
	free(cmptr);
	return cfd;
}

#endif // __TRANS_FD_H__

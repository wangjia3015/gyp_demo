#include <stdio.h>
#include <unistd.h>
#include "../socket/sockettools.h"
#include "unp.h"




#define MAX 16384

// client [IP] [PORT] [PROCESS] [LOOPS] [BYTES] 
int main(int argc, char ** argv) {
	if (argc != 6) {
		log("client IP PORT PROCESS LOOPS BYTES");
		return 0;
	}

	char request[MAXLINE + 1];
	char reply[MAX];

	int nchildren = atoi(argv[3]);
	int nloops = atoi(argv[4]);
	int nbytes = atoi(argv[5]);

	nbytes = nbytes > MAXLINE ? MAXLINE : nbytes;

	nbytes -= 1;
	memset(request, 'a', nbytes);
	
	request[nbytes] = 0;

	// flush 出现问题
	printf("run nchildren %d loops %d bytes %d\nbytes %s\n", nchildren, nloops, nbytes, request);
  fflush(stdout);
	for (int i = 0 ; i < nchildren; i++) {
		int pid = fork();
		if(pid == 0) {
			for(int j = 0; j < nloops; ++j) {
				int fd = tcp_connect(argv[1], argv[2]);

				if (fd < 1) {
					err_log("tcp_connect fd error");
					break;
				}

				write(fd, request, nbytes);

				int n = 0;
				if(( n = readn(fd, reply, nbytes)) != nbytes) {
					err_log("readn error");
				}

				close(fd);
			}
			printf("child %d done\n", i);
			exit(0);
		}
	}
	return 0;
}

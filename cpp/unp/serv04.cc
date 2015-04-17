#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../socket/sockettools.h"
#include "my_file_lock.h"


int child_make(int sfd, int index) {
	int pid;
	if ((pid = fork()) == 0) { // child
		int cfd;
		for(;;) {
			// lock 
			tools::my_file_lock();
			cfd = accept(sfd, NULL, NULL);
			tools::my_file_unlock();
			// unlock
			if (cfd <= 0) {
				err_log("error accept");
				continue;
			}
			sock_deal_client(cfd);
			close(cfd);
		}
		return 0;
	}	else {
		return pid;
	}
}



int main() {

	tools::my_file_lock_init();

	const int child_count = 5;

	int sfd = socket_tcp_server(8888);
	
	printf("fd %d\n", sfd);
	for(int i = 0 ; i < child_count ; ++i) {
		if (child_make(sfd, i) == 0) {
			return 0;
		}
	}
	
	// wait for child end
	int status;
	while(waitpid(-1, &status, WNOHANG) <= 0);

	return 0;
}

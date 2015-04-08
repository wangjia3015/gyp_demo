#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sockettools.h"

int main() {
	int sfd = socket_tcp_server(8888);
	
	/* 1 */
	//sock_select_mode(sfd);
	
	/* 2 */
	sock_epoll_mode(sfd);

	/* 3 */
	//for(;;) {
	//	int cfd = accept(sfd, NULL, NULL);
	//	sock_send_msg(cfd);
	//	close(cfd);
	//}

	return 0;
}

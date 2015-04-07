#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sockettools.h"

int main() {
	int sfd = socket_tcp_server(8888);
	
	set_socket_reuse(sfd);

	sock_select_mode(sfd);
//	for(;;) {
//		int cfd = accept(sfd, NULL, NULL);
//		sock_send_msg(cfd);
//		close(cfd);
//	}

	return 0;
}

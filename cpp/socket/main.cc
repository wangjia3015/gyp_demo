#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sockettools.h"

int main() {
	int sfd = socket_tcp_server(8888);
	sock_select_mode(sfd);
	return 0;
}

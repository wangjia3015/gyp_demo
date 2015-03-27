#include <stdio.h>
#include <arpa/inet.h>
#include "socktools.h"

void set_address(char *hname, char *sname, struct sockaddr_in * addr, char * protocol) {

}

int main() {
	//const char * host_name = "";
	int port = 8888;

	struct sockaddr_in addr;
	int server = socket(AF_INET, SOCK_STREAM, 0);

	if (server < 0) {
		printf("socket error %d\n", server);
		return 0;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr  = htonl(INADDR_ANY);

	int ret = bind(server, (const struct sockaddr *) &addr, sizeof(addr));
	if (ret < 0) {
		printf("bind error \n");
		return 0;
	}

	ret = listen(server, 1024);
	if (ret < 0) {
		printf("listen error \n");	return 0;
		return 0;               
	}

	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	for(;;) {
		printf("accept...\n");
		ret = accept(server, (struct sockaddr *) &client_addr, &len);
		//printf("accept from client %d \n", ret);
		show_socket_info(&client_addr);
		close(ret);
	}

	return 0;
}

#include <sys/types.h>

struct fd_state {
	char buffer[];
	size_t buffer_uesd;

	int writing;
	size_t n_written;
	size_t write_upto;
}

void run() {
	int sfd;
	fd_state *state[FD_SIZE];
	fd_set fdread, fdwrite;
	sinaddr_in addr;
	int max_fd;

	for (int i = 0 ; i < FD_SIZE; ++i) {
		state[i] = NULL;
	}

//	addr.sin_family = AF_INET;
//	addr.sin_port = 8888;
//	addr.sin_addr.s_addr = htonl(INADDR_ANY);


	sfd = socket_tcp_server(8888);

	make_nonblocking(sfd);

	max_fd = sfd;
	for(;;) {
		FD_ZERO(&fdread);
		FD_ZERO(&fdwrite);
		
		for(int i = 0 ; i < FD_SIZE; ++i) {
			if () {
			
			}
		}

	}
}

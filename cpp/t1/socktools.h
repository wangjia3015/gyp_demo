#include <arpa/inet.h>



void log(const char * msg) {
	printf("msg : %s\n", msg);
}

void show_socket_info(struct sockaddr_in * addr) {
	int port = ntohs(addr->sin_port);
	char buf[INET_ADDRSTRLEN];
	const char * ptr = inet_ntop(AF_INET, (struct sockaddr *)&addr->sin_addr, buf, INET_ADDRSTRLEN);
	if(ptr != NULL) {
		printf("socket info IP %s PORT %d\n", ptr, port);
	} else {
		printf("inet_ntop error\n");
	}
}

int initserver(int type, struct sockaddr * addr, int addrlen, int qlen) {
	int fd;
	int err = 0 ;
	if ((fd = socket(addr->sa_family, type, 0) < 0) {
		return -1;		
	} 

	if(bind(fd, addr, addrlen) < 0) {
		return -1;
	}

	if(listen(fd, addr, qlen) < 0) {
		return -1;
	}
	
	return fd;
}

void server(int sockfd) {
	for(;;) {
		int clientfd = accept(sockfd, NULL, NULL);
	}
}


void ruptime_server() {

	struct addrinfo hint;
	struct *aip, *alist;
	int socketfd, err, n;
	char * host;

	n = HOST_NAME_MAX;
	host = malloc(n);

	if(gethostname(host, n) < 0) {
		log("gethostname error");
	}

	memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_CANONNAME;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;

	if ((err = getaddrinfo(host, "ruptime", &hint, &ailist)) != 0) {
		log("getaddrinfo error");
		exit(1);
	}

	for(aip = ailist; aip != NULL; aip = aip->ai_next) {
		if ((sockfd = initserver(SOCK_STREAM, aip->ai_addr, aip->ai_addrlen) >= 0) {
				server(sockfd);
		}
	}

}

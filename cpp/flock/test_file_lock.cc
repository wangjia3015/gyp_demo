#include <stdio.h>


#include "file_lock.h"


int main() {
	const char * tmp = "/tmp/flockXXXXXX";
	my_lock_init(tmp);

	if (fork() == 0) {
		printf("do sub thing...\n");
	} else {
		printf("do main thing...\n");
	}

	
	sleep(1024);
	return 0;
}

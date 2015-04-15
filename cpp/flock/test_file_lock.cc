#include <stdio.h>


#include "file_lock.h"


int main() {
	const char * tmp = "/tmp/flockXXXXXX";
	my_lock_init(tmp);

	if (fork() == 0) {
		printf("do sub thing...\n");
    my_lock();
		printf("enter sub thing...\n");
	} else {
		printf("do main thing...\n");
    my_lock();
		printf("enter main thing...\n");
    for(int i = 0; i < 5; ++i) {
		  printf("do main thing...\n");
      sleep(1);
    }
		printf("end main thing...\n");
    my_unlock();
	}

	
	sleep(10);
	return 0;
}

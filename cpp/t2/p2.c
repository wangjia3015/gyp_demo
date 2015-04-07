#include <stdio.h>
#include <unistd.h>




int main () {

	//FILE * file = popen("ls /data/working/", "w");

	char buf[1024];
	int n = 0;
	while( ( n = fgets(buf, 1024, STDIN_FILENO)) > 0) {
		printf(buf);
		//write(STDOUT_FILENO, buf, n);
	} 
	return 0;
}

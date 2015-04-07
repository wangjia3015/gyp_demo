#include <stdio.h>
#include <unistd.h>




int main () {

	FILE * file = popen("ls /data/working/", "w");

	char buf[1024];
	int n = 0;
	while(( n = read(buf, 1024, file)) > 0) {
		printf(buf);
		//write(STDOUT_FILENO, buf, n);
	} 
	pclose(file);
	return 0;
}

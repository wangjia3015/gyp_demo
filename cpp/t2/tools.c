#include <stdio.h>


int is_unbuffered(FILE * fp) {
	return (fp->_flags & _IO_UNBUFFERED);
}

int is_linebuffered(FILE * fp) {
	return (fp->_flags & _IO_LINE_BUF);
}

int buffer_size(FILE * fp) {
	return (fp->_IO_buf_end - fp->_IO_buf_base);
}


void pr_stdio(const char * ptr, FILE * fp) {
	printf("stream = %s, ", ptr);
	if (is_unbuffered(fp)) {
		printf("unbuffered");	
	} else if (is_linebuffered(fp)) {
		printf("linebuffered");	
	} else {
		printf("fullbuffered");	
	}
	printf(", buffer size = %d \n", buffer_size(fp));
}

int main() {

	pr_stdio("stdin", stdin);
	pr_stdio("stdout", stdout);
	pr_stdio("stderr", stderr);

	return 0;
}

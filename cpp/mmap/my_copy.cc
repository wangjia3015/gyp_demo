#include <stdio.h>

// exit
#include <stdlib.h>

#include "../socket/sockettools.h"

#include <fcntl.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc, char ** argv) {
	const static int BUFFER_SIZE = 1024 * 1024 * 64;
	int fdin;
	int fdout;
	void * src;
	void * dst;
	struct stat sinfo;
	size_t trans_size = 0;

	if((fdin = open(argv[1], O_RDONLY)) < 0) {
		err_log("src file open error");
		return 0;
	}

	//if((fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE)) < 0) {
	if((fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC)) < 0) {
		err_log("dst file open error");
		return 0;
	}

	if(fstat(fdin, &sinfo) < 0) {
		err_log("fstat err");
		return 0;
	}

	if(ftruncate(fdout, sinfo.st_size) < 0) {
		err_log("ftruncate err");
		return 0;
	}

	printf("st_size %d\n", sinfo.st_size);
	
	while(trans_size < sinfo.st_size) {
		int mem_size = sinfo.st_size - trans_size;	
		mem_size = mem_size > BUFFER_SIZE? BUFFER_SIZE :	mem_size;
		printf("alloc %d", mem_size);

		if ((src = mmap(0, mem_size, PROT_READ, MAP_PRIVATE, fdin, trans_size)) == MAP_FAILED) {
			err_log("create src mmap error");
			exit(0);
		}
		if ((dst = mmap(0, mem_size, PROT_READ | PROT_WRITE , MAP_SHARED, fdout, trans_size)) == MAP_FAILED) {
			err_log("create dst mmap error");	
			exit(0);
		} 
		memcpy(dst, src, mem_size);
		munmap(src, mem_size);
		munmap(dst, mem_size);

		trans_size += mem_size;
	}

	printf("%d transfered\n", trans_size);
	return 0;
}

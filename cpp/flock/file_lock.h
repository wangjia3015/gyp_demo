#ifndef __FILE_LOCK_H__
#define __FILE_LOCK_H__

#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include "../socket/sockettools.h"
static flock lock_it, unlock_it;
static int lock_fd = -1;

void my_lock_init(const char *pathname) {
	char lock_file[1024];
	strncpy(lock_file, pathname, 1024);
	log(lock_file);
	lock_fd = mkstemp(lock_file);
	assert(lock_fd > 0);

	lock_it.l_type = F_WRLCK;
	lock_it.l_len = 0;
	lock_it.l_start = 0;
	lock_it.l_whence = SEEK_SET;
	
	unlock_it.l_type = F_UNLCK;
	unlock_it.l_len = 0;
	unlock_it.l_start = 0;
	unlock_it.l_whence = SEEK_SET;

}

int my_try_lock() {
	int ret = fcntl(lock_fd, F_SETLK, &lock_it);	
	if(ret < 0) {
		return -1;
	} else {
		return 0;
	}
}

void my_lock() {
	if(fcntl(lock_fd, F_SETLKW, &lock_it) < 0) {
		err_log("");
	}
}

void my_unlock() {
	if(fcntl(lock_fd, F_SETLKW, &unlock_it) < 0) {
		err_log("unlock error");
	}
}

#endif // __FILE_LOCK_H__

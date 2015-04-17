#ifndef __MY_FILE_LOCK_H__
#define __MY_FILE_LOCK_H__

namespace tools {

static flock lock_base;
static flock unlock_base;
static int lock_fd = -1;

void my_file_lock_init() {
	char tmp[] = "/tmp/file_lockXXXXXX"; 
	lock_fd = mkstemp(tmp);
	if(lock_fd < 0) {
		err_log("mkstemp error");
		return ;
	}

	lock_base.l_type = F_WRLCK;
	lock_base.l_whence = SEEK_SET;
	lock_base.l_start = 0;
	lock_base.l_len = 0;

	unlock_base.l_type = F_UNLCK;
	unlock_base.l_whence = SEEK_SET;
	unlock_base.l_start = 0;
	unlock_base.l_len = 0;

}

void my_file_lock() {
	if((fcntl(lock_fd, F_SETLKW, &lock_base)) < 0) {
		err_log("file_lock error");
	}
}

void my_file_unlock() {
	if((fcntl(lock_fd, F_SETLKW, &unlock_base)) < 0) {
		err_log("file_lock error");
	}
}

}

#endif // __MY_FILE_LOCK_H__

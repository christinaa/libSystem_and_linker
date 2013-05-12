/*
 * Core Framework: LibSyscall: darwin_stat.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Stat shim.
 * XXX: This should be in the kernel.
 */


#include <stdio.h>
#include <sys/stat.h>

#include "_lnk_log_c.h"

/*
 * Linux struct stat (sizeof = 96)
 * Make sure the kernel uses Darwin ABI.
 */
struct LX_stat {
	unsigned long long      st_dev;
	unsigned char   __pad0[4];

#define STAT64_HAS_BROKEN_ST_INO        1
	unsigned long   __st_ino;
	unsigned int    st_mode;
	unsigned int    st_nlink;

	unsigned long   st_uid;
	unsigned long   st_gid;

	unsigned long long      st_rdev;
	unsigned char   __pad3[4];

	long long       st_size;
	unsigned long   st_blksize;
	unsigned long long st_blocks; 

	unsigned long   LX_st_atime;
	unsigned long   st_atime_nsec;

	unsigned long   LX_st_mtime;
	unsigned long   st_mtime_nsec;

	unsigned long   LX_st_ctime;
	unsigned long   st_ctime_nsec;

	unsigned long long      st_ino;
};


/* Syscalls */
extern int stat$LINUX(const char *, struct LX_stat *);
extern int lstat$LINUX(const char *, struct LX_stat *);
extern int fstat$LINUX(int, struct LX_stat *);

int	fstat(int fd, struct stat * dst) {
	struct LX_stat st;
	
	//lnkPnum("fstat_user(): sizeof stat64: ", sizeof(struct LX_stat));
	
	/* perform linux syscall */
	int rr = fstat$LINUX(fd, &st);
	
	/* translate */
	dst->st_dev = st.st_dev;
	dst->st_mode = st.st_mode;
	dst->st_nlink = st.st_nlink;
	dst->st_ino = st.st_ino;
	dst->st_uid = st.st_uid;
	dst->st_gid = st.st_gid;
	dst->st_rdev = st.st_rdev;
	dst->st_size = st.st_size;
	dst->st_blksize = st.st_blksize;
	dst->st_blocks = st.st_blocks;

	dst->st_atime = st.LX_st_atime;
	dst->st_mtime = st.LX_st_mtime;
	dst->st_ctime = st.LX_st_ctime;

	return rr;
}

int	lstat(const char * path, struct stat * dst) {
	struct LX_stat st;
	
	/* perform linux syscall */
	int rr = lstat$LINUX(path, &st);
	
	/* translate */
	dst->st_dev = st.st_dev;
	dst->st_mode = st.st_mode;
	dst->st_nlink = st.st_nlink;
	dst->st_ino = st.st_ino;
	dst->st_uid = st.st_uid;
	dst->st_gid = st.st_gid;
	dst->st_rdev = st.st_rdev;
	dst->st_size = st.st_size;
	dst->st_blksize = st.st_blksize;
	dst->st_blocks = st.st_blocks;
	
	dst->st_atime = st.LX_st_atime;
	dst->st_mtime = st.LX_st_mtime;
	dst->st_ctime = st.LX_st_ctime;
	
	return rr;
}

int	stat(const char * path, struct stat * dst) {
	struct LX_stat st;
	
	/* perform linux syscall */
	int rr = stat$LINUX(path, &st);
	
	/* translate */
	dst->st_dev = st.st_dev;
	dst->st_mode = st.st_mode;
	dst->st_nlink = st.st_nlink;
	dst->st_ino = st.st_ino;
	dst->st_uid = st.st_uid;
	dst->st_gid = st.st_gid;
	dst->st_rdev = st.st_rdev;
	dst->st_size = st.st_size;
	dst->st_blksize = st.st_blksize;
	dst->st_blocks = st.st_blocks;
	
	dst->st_atime = st.LX_st_atime;
	dst->st_mtime = st.LX_st_mtime;
	dst->st_ctime = st.LX_st_ctime;
	
	return rr;
}

// newline! \n


/*
 * Credentials
 * Copyright (c) 2012 Christina Brooks
 *
 * Supplies information about users and groups.
 */

/*
	XXX
	this probably needs a better name
	and a better implementation
 */

#include <stdio.h>
#include <grp.h>
#include <pwd.h>
#include <netdb.h>
#include <stdlib.h>

#include "OSLog.h"

#define bb_log(x, ...) OSLogLib("boring", x, ##__VA_ARGS__)
#define bb_not_implemented() OSHalt("boring: (%s:%d) not implemented ", __FUNCTION__, __LINE__)
#define bb_trace() OSLog("boring: TRACE: file %s, line %d", __FUNCTION__, __LINE__)
#define bb_error_trace() OSLog("boring: ERROR: file %s, line %d", __FUNCTION__, __LINE__)

struct group *
getgrent(void) {
	bb_trace();
	return NULL;
}

struct group *
getgrnam(const char *name) {
	bb_trace();
	return NULL;
}

int
getgrnam_r(const char *name, struct group *grp, char *buffer, size_t bufsize, struct group **result) {
	bb_trace();
	return 1;
}

struct group *
getgrgid(gid_t gid) {
	bb_trace();
	return NULL;
}

int
getgrgid_r(gid_t gid, struct group *grp, char *buffer, size_t bufsize, struct group **result) {
	bb_trace();
	return -1;
}

int
setgroupent(int stayopen) {
	bb_trace();
	return 1;
}

void
setgrent(void) {
	bb_trace();
}

void
endgrent(void) {
	bb_trace();
}


struct servent *
getservent()
{
	bb_trace();
	return NULL;
}

struct servent *
getservbyname(const char *name, const char *proto)
{
	bb_trace();
	return NULL;
}

struct servent *
getservbyport(int port, const char *proto)
{
	bb_trace();
	return NULL;
}

void
setservent(int stayopen)
{
	bb_trace();
}

void
endservent(void)
{
	bb_trace();
}


struct passwd *
getpwent(void) {
	bb_trace();
	return NULL;
}

struct passwd *
getpwnam(const char *login) {
	bb_trace();
	return NULL;
}

int
getpwnam_r(const char *name, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result) {
	bb_trace();
	return 1;
}

struct passwd *
getpwuid(uid_t uid) {
	bb_trace();
	/*
		bash uses this, so lets make a stub for now
	 
	 current_user.user_name = savestring (entry->pw_name);
	 current_user.shell = (entry->pw_shell && entry->pw_shell[0])
	 ? savestring (entry->pw_shell)
	 : savestring ("/bin/sh");
	 current_user.home_dir = savestring (entry->pw_dir);
	 */
	
	struct passwd* pp = calloc(1, sizeof(struct passwd));
	pp->pw_name = "root";
	pp->pw_dir = "/";
	
	return pp;
}

int
getpwuid_r(uid_t uid, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result) {
	bb_trace();
	return -1;
}

int
setpassent(int stayopen) {
	bb_trace();
	return -1;
}

void
setpwent(void) {
	bb_trace();
}

void
endpwent(void) {
	bb_trace();
}

void *fgetgrent(FILE *stream)
{
	bb_trace();
	return NULL;
}

// newline! \n


/*
 * Core Framework: Mach
 * Copyright (c) 2012 Christina Brooks
 *
 * Mach layer.
 */

/*
	Yes, I know, this is shit.
	I'm not too bothered about the mach port semantics.
	This works. It's insecure, but it works. Now go away.
 */

#include <stdio.h>
#include "_lnk_log_c.h"
#include <mach/message.h>
#include <stdlib.h>
#include <mach/mach.h>
#include <pthread.h>
#include "OSLog.h"

#define mach_ipc_log(x, ...) OSLogLib("mach_ipc", x, ##__VA_ARGS__)

#define warn_unimpl() mach_ipc_log("%s(): unimplemented but returning KERN_SUCCESS", __FUNCTION__)

/*
	 These mach messages are sent to the kernel for IPC
	 related tasks. These messages are special, as they're not
	 mach messages per-se.
 */
#define KMSG_MACH_PORT_ALLOCATE 2000
#define KMSG_GET_DIRECTORY_ENTRIES 2002

typedef struct 
{
	mach_msg_header_t head;
	mach_port_right_t rights;
	
	mach_port_t* port_out;
} kmsg_mach_port_allocate_msg_t;

typedef struct 
{
	mach_msg_header_t head;
	
	int fd;
	int* out_error;
	void* buffer;
	unsigned int buffer_len;
} kmsg_get_directory_entries_t;

/* Helper macros to create kmsgs */
#define fill_kmsg_header(h, msgid, sz) h->msgh_bits=0;\
	h->msgh_size=sz;\
	h->msgh_id=msgid;\
	h->msgh_local_port=0;\
	h->msgh_remote_port=0;

#define kmsg_send(prt, h) mach_msg((mach_msg_header_t*)h, MACH_SEND_MSG, 0, 0, prt, 0, 0);


char *
mach_error_string(mach_error_t err)
{
	return "unknown error (xxx)";
}

mach_port_t mach_reply_port(void)
{
	mach_port_t p;
	mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &p);
	
	return p;
}

kern_return_t mach_port_set_mscount(ipc_space_t task, mach_port_name_t name, mach_port_mscount_t mscount)
{
	warn_unimpl();
	return KERN_SUCCESS;
}

kern_return_t mach_port_move_member(ipc_space_t task, mach_port_name_t member, mach_port_name_t after)
{
	warn_unimpl();
	return KERN_SUCCESS;
}

kern_return_t mach_port_get_set_status(ipc_space_t task,
									   mach_port_name_t name,
									   mach_port_name_array_t *members,
									   mach_msg_type_number_t *membersCnt)
{
	warn_unimpl();
	return KERN_SUCCESS;
}

kern_return_t mach_port_request_notification(ipc_space_t task,
											 mach_port_name_t name,
											 mach_msg_id_t msgid,
											 mach_port_mscount_t sync,
											 mach_port_t notify,
											 mach_msg_type_name_t notifyPoly,
											 mach_port_t *previous)
{
	warn_unimpl();
	return KERN_SUCCESS;
}

mach_msg_return_t mach_msg_send(mach_msg_header_t *h)
{
	return mach_msg(h, MACH_SEND_MSG, 0, 0, h->msgh_remote_port, 0, 0);
}

kern_return_t mach_port_set_attributes(ipc_space_t task,
									   mach_port_name_t name,
									   mach_port_flavor_t flavor,
									   mach_port_info_t port_info,
									   mach_msg_type_number_t port_infoCnt)
{
	warn_unimpl();
	return KERN_SUCCESS;
}

kern_return_t mach_port_extract_member(ipc_space_t task, mach_port_name_t name, mach_port_name_t pset)
{
	warn_unimpl();
	return KERN_SUCCESS;
}

void mach_msg_destroy(mach_msg_header_t*msg) 
{
	
}

kern_return_t mach_port_get_attributes(ipc_space_t task,
									   mach_port_name_t name,
									   mach_port_flavor_t flavor,
									   mach_port_info_t port_info_out,
									   mach_msg_type_number_t *port_info_outCnt)
{
	warn_unimpl();
	return KERN_SUCCESS;
}

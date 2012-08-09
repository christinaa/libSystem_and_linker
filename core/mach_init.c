/*
 * Core Framework: MachInit
 * Copyright (c) 2012 Christina Brooks
 *
 * Mach init stuff. Inits the mach subsystem.
 */

#include <stdio.h>
#include <mach/mach.h>
#include "kernelrpc.h"

mach_port_t	mach_task_self_ = MACH_PORT_NULL;
mach_port_t mach_host_self_ = MACH_PORT_NULL;
mach_port_t bootstrap_port = MACH_PORT_NULL;

vm_size_t vm_page_size;
vm_size_t vm_page_mask;
int	vm_page_shift;

/*
	NDR stands for Network Data Representation.
	That's all I know ...
 */
NDR_record_t NDR_record;

static boolean_t mach_init_inited = FALSE;

extern mach_port_t task_self(void);

int mach_init_doit(int forkchild)
{
	vm_page_size = PAGE_SIZE;
	
	/*
		Task port.
	 */
	mach_task_self_ = task_self();
	
	return 0;
}

/* 
	This is called by OSInitializer by lnk.
 */
int mach_init(void)
{
	int ret;
	
	if (mach_init_inited)
		return(0);
	
	mach_init_inited = TRUE;
	ret = mach_init_doit(0);
	
	return ret;
}


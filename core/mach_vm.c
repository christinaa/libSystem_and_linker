/*
 * Core Framework: Mach VM
 * Copyright (c) 2012 Christina Brooks
 *
 * Mach VM stuff.
 */

#include <stdio.h>
#include <mach/mach.h>
#include <sys/mman.h>
#include "OSLog.h"

kern_return_t
vm_deallocate(vm_map_t target_task, vm_address_t address, vm_size_t size)
{
	if (target_task == mach_task_self()) {
		int ret =
		munmap((void*)address,
			   (size_t)size);
		
		if (ret == 0) {
			return KERN_SUCCESS;
		}
		else {
			return KERN_FAILURE;
		}
	}
	else {
		OSHalt("vm_deallocate tried to deallocate memory for a foreign task");
	}
}

kern_return_t vm_remap
(
 vm_map_t target_task,
 vm_address_t *target_address,
 vm_size_t size,
 vm_address_t mask,
 int flags,
 vm_map_t src_task,
 vm_address_t src_address,
 boolean_t copy,
 vm_prot_t *cur_protection,
 vm_prot_t *max_protection,
 vm_inherit_t inheritance
 )
{
	OSHalt("vm_remap: Not implemented.");
}
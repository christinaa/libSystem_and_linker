/*
 * core: lnk_cas.cpp
 * Copyright (c) 2012 Christina Brooks
 *
 * Casper.
 */

#include <stdio.h>
#include "lnk_cas.h"
#include <sys/socket.h>

static cas* _sharedInstance = NULL;

cas* cas::get()
{
	if (_sharedInstance == NULL) {
		_sharedInstance = new cas();
		_sharedInstance->fModuleName = "Casper";
	}
	
	return _sharedInstance;
}

void cas::init()
{
#ifndef _OPEN_SOURCE
	const char* path = "/usr/lib/bunny.dylib";
	fLibrary = (MachObject*)lnk::loadLibrary(path);
#endif
}

ImageType cas::getImageType()
{
	return kImageTypeCasper;
}

bool cas::active()
{
	return false;
}

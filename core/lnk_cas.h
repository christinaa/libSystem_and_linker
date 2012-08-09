/*
 * core: lnk_cas.h
 * Copyright (c) 2012 Christina Brooks
 *
 * Casper.
 */

#ifndef core_lnk_cas_h
#define core_lnk_cas_h

#include "lnk.h"
#include "lnk_macho.h"

class cas : public Image {
	
public:
	virtual ImageType getImageType();
	
	static cas* get();
	void init();
	
	bool active();
	bool load_img(const char* ip);
	
protected:
	MachObject* fLibrary;
};

#endif

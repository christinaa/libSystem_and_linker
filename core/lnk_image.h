/*
 * Core Framework: Linker: lnk_image.h
 * Copyright (c) 2012 Christina Brooks
 *
 * Image.
 */

#ifndef core_lnk_image_h
#define core_lnk_image_h

#include "macho.h"

typedef enum {
	kImageTypeUnknown = 0,
	kImageTypeMachO = 1,
	kImageTypeElf = 2,
	kImageTypeBridge = 3,
	kImageTypeCasper = 4
} ImageType;

/* Bridge Support */
typedef struct __BridgeEntry {
	const unsigned mode;
} BridgeEntry;

#define BRIDGE_SOCKETS 1

class Image {
	
protected:
	Image* fNextImage;
	Image* fPrevImage;
	
	int fImageType;
	const char* fFilePath;
	const char* fOrigFilePath;
	const char* fModuleName;
	
	/* Um. 128 is enough. I guess. */
	Image* fDependencies[128];
	int fDepCount;
	
	bool fHasInitialized;
	
public:
	//Image();
	
	void setPrevImage(Image* image);
	void setNextImage(Image* image);
	Image* nextImage();
	Image* prevImage();
	
	bool dependenciesInitialized();
	bool hasInitialized();
	
	Image** getDependencies(int* count);
	bool dependsOn(Image* image);
	
	virtual bool findExportedSymbol(const char* symbol, Symbol* sym);
	virtual uintptr_t exportedSymbolAddress(Symbol* sym);
	
	virtual void doRebase();
	virtual void doBindSymbols();
	virtual void doInitialize();
	
	virtual void init();
	
	virtual const char* filePath();
	virtual const char* origFilePath();
	
	virtual const char* getShortName();
	virtual uintptr_t getSlide();
	
	virtual ImageType getImageType();
	virtual BridgeEntry* getBridgeEntry();
};

struct ProgramVars
{
	const void*		mh;
	int*			NXArgcPtr;
	const char***	NXArgvPtr;
	const char***	environPtr;
	const char**	__prognamePtr;
};

typedef void (*Initializer)(int argc, const char* argv[], const char* envp[], const char* apple[], const ProgramVars* vars);

#endif

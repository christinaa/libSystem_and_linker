/*
 * Core Framework: Linker: lnk_image.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Abstract binary image class used in the linking chain.
 */

#include <stdio.h>
#include "lnk_image.h"
#include "lnk.h"
#include <string.h>

bool Image::dependenciesInitialized()
{
	for (int i = 0; i < fDepCount; i++) 
	{
		Image* dep = fDependencies[i];
		
		if (!dep->fHasInitialized)
			return false;
	}
	
	return true;
}

void Image::init() {
}

bool Image::dependsOn(Image* image)
{
	for (int i = 0; i < fDepCount; i++) 
	{
		if (fDependencies[i] == image)
		{
			return true;
		}
	}
	
	return false;
}

Image** Image::getDependencies(int* count)
{
	*count = fDepCount;
	return fDependencies;
}

void Image::setPrevImage(Image* image) {
	fPrevImage = image;
}

Image* Image::prevImage() {
	return fPrevImage;
}

void Image::setNextImage(Image* image) {
	fNextImage = image;
}

Image* Image::nextImage() {
	return fNextImage;
}

bool Image::hasInitialized()
{
	return fHasInitialized;
}

bool Image::findExportedSymbol(const char* symbol, Symbol* sym)
{
	return NULL;
}

uintptr_t Image::exportedSymbolAddress(Symbol* sym) {
	return 0;
}

void Image::doRebase() {
}

void Image::doBindSymbols() {
}

BridgeEntry* Image::getBridgeEntry() {
	return NULL;
}

uintptr_t Image::getSlide() {
	return 0;
}

void Image::doInitialize() {
}

const char* Image::filePath() {
	return fFilePath;
}

const char* Image::origFilePath() {
	return fOrigFilePath;
}

ImageType Image::getImageType()
{
	return kImageTypeUnknown;
}

const char* Image::getShortName()
{
	if (fModuleName != NULL) {
		return fModuleName;
	}
	if (fFilePath != NULL) {
		const char* s = strrchr(fFilePath, '/');
		if (s != NULL) 
			return &s[1];
	}
	return fFilePath; 
}
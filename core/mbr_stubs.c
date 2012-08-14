/*
 * core: mbr_stubs.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Memberships are not supported on this platform.
 * Implements the minimal support for UUID<->String conversion.
 */

#include <stdio.h>
#include <membership.h>
#include "OSLog.h"
#include <string.h>
#include <errno.h>

#define MBR_UU_STRING_SIZE 37
#define MBR_MAX_SID_STRING_SIZE 200

static void ConvertBytesToHex(char** string, char** data, int numBytes)
{
	int i;
	
	for (i=0; i < numBytes; i++)
	{
		unsigned char hi = ((**data) >> 4) & 0xf;
		unsigned char low = (**data) & 0xf;
		if (hi < 10)
			**string = '0' + hi;
		else
			**string = 'A' + hi - 10;
		
		(*string)++;
		
		if (low < 10)
			**string = '0' + low;
		else
			**string = 'A' + low - 10;
		
		(*string)++;
		(*data)++;
	}
}

int mbr_uuid_to_string(const uuid_t uu, char* string)
{
	char* guid = (char*)uu;
	char* strPtr = string;
	ConvertBytesToHex(&strPtr, &guid, 4);
	*strPtr = '-'; strPtr++;
	ConvertBytesToHex(&strPtr, &guid, 2);
	*strPtr = '-'; strPtr++;
	ConvertBytesToHex(&strPtr, &guid, 2);
	*strPtr = '-'; strPtr++;
	ConvertBytesToHex(&strPtr, &guid, 2);
	*strPtr = '-'; strPtr++;
	ConvertBytesToHex(&strPtr, &guid, 6);
	*strPtr = '\0';
	
	return 0;
}

int mbr_string_to_uuid(const char* string, uuid_t uu)
{
	short dataIndex = 0;
	int isFirstNibble = 1;
	
	if (strlen(string) > MBR_UU_STRING_SIZE)
		return EINVAL;
	
	while (*string != '\0' && dataIndex < 16)
	{
		char nibble;
		
		if (*string >= '0' && *string <= '9')
			nibble = *string - '0';
		else if (*string >= 'A' && *string <= 'F')
			nibble = *string - 'A' + 10;
		else if (*string >= 'a' && *string <= 'f')
			nibble = *string - 'a' + 10;
		else
		{
			if (*string != '-')
				return EINVAL;
			string++;
			continue;
		}
		
		if (isFirstNibble)
		{
			uu[dataIndex] = nibble << 4;
			isFirstNibble = 0;
		}
		else
		{
			uu[dataIndex] |= nibble;
			dataIndex++;
			isFirstNibble = 1;
		}
		
		string++;
	}
	
	if (dataIndex != 16)
		return EINVAL;
	
	return 0;
}

int
mbr_uuid_to_id(const uuid_t uu, uid_t *id, int *id_type)
{
	return EIO;
}

int
mbr_gid_to_uuid(gid_t id, uuid_t uu)
{
	return EIO;
}

int
mbr_uid_to_uuid(uid_t id, uuid_t uu)
{
	return EIO;
}

// newline! \n


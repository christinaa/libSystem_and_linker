/*
 * Core Framework: Linker: lnk_debug
 * Copyright (c) 2012 Christina Brooks
 *
 * Linker debugging code. Don't use it unless you have
 * a good reason to. This is lower level debugging code that
 * is designed to work almost all the time and without
 * dynamically allocating any memory.
 */

#include "lnk_debug.h"

namespace lnk {
	namespace ldbg {
		void printText(const char* text) {
			write(0, text, strlen(text));
		}
		
		void printNumber(const char* desc, int anum) {
			char buffer[4096];
			unsigned int num = anum;
			
			/* in memcpy.s */
			memcpy(buffer, desc, strlen(desc));
			int bufpos = strlen(desc);
			
			if (num == 0) {
				buffer[bufpos] = '0';
				bufpos += 1;
			}
			else {
				char rev[128]; /* more than enough */
				int i = 0;
				
				while (num > 0) {
					int ss = num % 10;
					num /= 10;
					char c = 48 + ss;
					
					rev[i] = c;
					i++;
				}
				
				for (; i >= 0; i--) {
					buffer[bufpos] = rev[i];
					bufpos += 1;
				}
			}
			
			/* newline */
			buffer[bufpos] = '\n';
			bufpos += 1;
			
			write(0, &buffer, bufpos);
		}
	}
	
	namespace dbg {
		
	}
}
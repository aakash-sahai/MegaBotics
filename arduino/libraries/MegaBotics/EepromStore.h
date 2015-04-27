/*
 * This file is part of the support libraries for the Arduino based MegaBotics
 * platform.
 *
 * Copyright (c) 2015, Aakash Sahai and other contributors, a list of which is
 * maintained in the associated repository. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    + Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    + Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    + Neither the name of the copyright holder nor the names of its contributors
 *	may be used to endorse or promote products derived from this software
 *	without specific prior written permission.
 */

/*
 * EepromStrore.h
 *
 *  Created on: Apr 20, 2015
 *      Author: Aakash Sahai
 */

#ifndef MEGABOTICS_EEPROMSTRORE_H_
#define MEGABOTICS_EEPROMSTRORE_H_

#include <avr/eeprom.h>
#include "LinkedList.h"

class EepromStore {
public:
	static const char sectionNameLength = 4;
	static const char signatureLength = 6;
	static const int eepromSize = 4096;

	enum Status {
		SUCCESS = 0,
		RW_FAILURE = 1,
		NO_SPACE = 2,
		NOT_FOUND = 3,
		TOO_BIG = 4,
		LENGTH_MISMATCH = 5
	};

	/*
	 * A six byte signature is stored at the beginning of EEPROM to indicate if it
	 * contains valid information compatible with this EpromStore implementation.
	 */
	static const char signature[signatureLength];

	/*
	 * - The information is stored in EEPROM in multiple sections of differing lengths.
	 * - The section starts with a 6 byte header.
	 * - Each section is identified by a unique 4 character name, followed by 2 byte field indicating
	 *   size of data stored and 2 byte field indicating total section Length.
	 */
	struct SectionHeader {
		char	name[sectionNameLength];
		int		dataLen;
		int		sectionLen;

		bool isFree() { return dataLen == 0; }
	};

	virtual ~EepromStore();

	/*
	 * Implemented as a Singleton object
	 */
	static EepromStore * getInstance() { return & _instance; }
	static EepromStore & getReference() { return _instance; }

	void setup(void);
	Status loadSection(char *name, void *buf, int *length);
	Status storeSection(char *name, void *buf, int length);
	Status deleteSection(char *name);
	void list(void);
	void listFree(void);

private:
	static EepromStore _instance;
	class SectionEntry : public ListNode {
	public:
		SectionEntry();
		virtual ~SectionEntry();
		SectionEntry *next() { return (SectionEntry *)_next; }
		SectionHeader header;				// The header info stored in EEPROM
		int			  offset;				// The offset in EEPROM where data is stored
		void print(void);
	};

	LinkedList	_occupied;					// The linked list of occupied sections
	LinkedList  _free;						// The linked list of free sections
	void list(LinkedList & aList);

	EepromStore();
	EepromStore(EepromStore const&);        // Don't Implement to disallow copy by assignment
    void operator=(EepromStore const&);		// Don't implement to disallow copy by assignment
	SectionEntry *findSection(const char *name);
};

#endif /* MEGABOTICS_EEPROMSTRORE_H_ */

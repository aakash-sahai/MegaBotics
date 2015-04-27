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
 * EepromStrore.cpp
 *
 *  Created on: Apr 20, 2015
 *      Author: Aakash Sahai
 */
#define DEBUG	0

#include "MegaBotics.h"
#include <string.h>
#include <Arduino.h>

EepromStore EepromStore::_instance;

const char EepromStore::signature[EepromStore::signatureLength] = { 'E', 'S', 'T', 'O', 'R', 'E' };

#define FREE_SECTION_NAME	"FREE"
#define MIN_FREE_BYTES		8

EepromStore::SectionEntry::SectionEntry() { offset = 0; }
EepromStore::SectionEntry::~SectionEntry() { }

EepromStore::EepromStore() {
	//  Nothing to do
}

EepromStore::~EepromStore() {
	//  Nothing to do
}

void EepromStore::setup(void) {
	char sig[signatureLength];
	int i;
	eeprom_read_block(sig, 0, signatureLength);
	for (i = 0; i < signatureLength; i++) {
		if (sig[i] != signature[i])
			break;
	}
	if (i != EepromStore::signatureLength) {
		DBG_PRINTLN(F("Did not find Signature"));
		// We didn't find Signatures in EEPROM; Write Signature and the FREE Section Header
		eeprom_update_block(signature, 0, EepromStore::signatureLength);
		SectionEntry *sptr = new SectionEntry;
		strncpy(sptr->header.name, FREE_SECTION_NAME, sectionNameLength);
		sptr->header.sectionLen = eepromSize - sizeof(SectionHeader) - signatureLength;
		sptr->header.dataLen = 0;
		sptr->offset = signatureLength + sizeof(SectionHeader);
		_free.insert(sptr);
		eeprom_update_block(&sptr->header, (void *)signatureLength, sizeof(SectionHeader));
		return;
	}
	// Signatures are valid; read and cache the _occupied and _free block list
	int offset = signatureLength;			// Skip past the signatures
	DBG_PRINTLN(F("Found Signature"));
	while (offset < eepromSize) {
		SectionEntry *sptr = new SectionEntry;
		eeprom_read_block(&sptr->header, (void *)offset, sizeof(SectionHeader));
		DBG_PRINTLN("Read Block");
		offset += sizeof(SectionHeader);	// Skip to the beginning of section
		sptr->offset = offset;
		if (sptr->header.isFree()) {
			_free.insert(sptr);
		} else {
			_occupied.insert(sptr);
		}
		offset += sptr->header.sectionLen;	// Skip to the next section header
	}
	DBG_PRINTLN(F("EEPROM Store Cache primed"));
}

EepromStore::Status EepromStore::loadSection(char *name, void *buf, int *length) {
	SectionEntry *sptr = findSection(name);
	if (sptr == 0)
		return NOT_FOUND;
	int dataLen = sptr->header.dataLen;
	if (*length < dataLen) {
		*length = dataLen;
		return TOO_BIG;
	}
	Status status = (dataLen == *length) ? SUCCESS : LENGTH_MISMATCH;
	*length = dataLen;
	eeprom_read_block(buf, (void *)sptr->offset, dataLen);
	return status;
}

EepromStore::Status EepromStore::storeSection(char *name, void *buf, int length) {
	SectionEntry *sptr = findSection(name);
	if (sptr == 0) {	// Not found. Allocate a free block
		for (sptr = (SectionEntry *)_free.first(); sptr != (SectionEntry *)NULL; sptr = sptr->next()) {
			if (sptr->header.sectionLen >= length)
				break;
		}
		if (sptr == (SectionEntry *)NULL) {
			return NO_SPACE;
		}

		unsigned int excess = sptr->header.sectionLen - length - sizeof(SectionHeader);
		if (excess < MIN_FREE_BYTES) {
			// Use the whole free block
			sptr->header.dataLen = length;
			strncpy(sptr->header.name, name, sectionNameLength);
			// Remove from the free list and insert into the occupied list
			_free.remove(sptr);
			_occupied.insert(sptr);
			// Write the section header and data to the EEPROM
			eeprom_update_block(&sptr->header, (void *)(sptr->offset-sizeof(SectionHeader)), sizeof(SectionHeader));
			eeprom_update_block(buf, (void *)sptr->offset, length);
		} else {
			// Chop the free block into two pieces
			// Shrink and retain the first piece as free block
			sptr->header.sectionLen = excess;
			// Use the second piece to store the new section
			SectionEntry *nptr = new SectionEntry;
			nptr->header.dataLen = length;
			nptr->header.sectionLen = length;
			strncpy(nptr->header.name, name, sectionNameLength);
			nptr->offset = sptr->offset + excess + sizeof(SectionHeader);
			// Insert the new section into the occupied list; leaving the first section in free list
			_occupied.insert(nptr);
			// Write both section headers and data to the EEPROM
			eeprom_update_block(&sptr->header, (void *)(sptr->offset-sizeof(SectionHeader)), sizeof(SectionHeader));
			eeprom_update_block(&nptr->header, (void *)(nptr->offset-sizeof(SectionHeader)), sizeof(SectionHeader));
			eeprom_update_block(buf, (void *)nptr->offset, length);
		}
		return SUCCESS;
	}

	if (length > sptr->header.sectionLen) {
		// Delete the existing block and retry storing
		deleteSection(name);
		return storeSection(name, buf, length);
	} else {	// Length is big enough to fit in section
		sptr->header.dataLen = length;
		eeprom_update_block(&sptr->header, (void *)(sptr->offset-sizeof(SectionHeader)), sizeof(SectionHeader));
		eeprom_update_block(buf, (void *)sptr->offset, length);
		return SUCCESS;
	}
}

EepromStore::SectionEntry * EepromStore::findSection(const char *name) {
	SectionEntry *sptr;
	for (sptr = (SectionEntry *)_occupied.first(); sptr != (SectionEntry *)NULL; sptr = sptr->next()) {
		if (strncmp(sptr->header.name, name, sectionNameLength) == 0) {
			return sptr;
		}
	}
	return NULL;
}

void EepromStore::list(void) {
	list(_occupied);
}

void EepromStore::listFree(void) {
	list(_free);
}

void EepromStore::list(LinkedList &aList) {
	SectionEntry *sptr;
	for (sptr = (SectionEntry *)aList.first(); sptr != (SectionEntry *)NULL; sptr = sptr->next()) {
		sptr->print();
	}
}

void EepromStore::SectionEntry::print(void) {
	char buf[80];
	strncpy(buf, header.name, 4);
	sprintf(buf+4, " - SLEN: %d DLEN: %d OFFSET: %d\n",
			header.sectionLen,
			header.dataLen, offset);
	Serial.print(buf);
}

EepromStore::Status EepromStore::deleteSection(char *name) {
	SectionEntry *sptr = findSection(name);
	if (sptr == 0) {
		return NOT_FOUND;
	}

	strncpy(sptr->header.name, FREE_SECTION_NAME, sectionNameLength);
	sptr->header.dataLen = 0;
	_occupied.remove(sptr);
	_free.insert(sptr);
	eeprom_update_block(&sptr->header, (void *)(sptr->offset - sizeof(SectionHeader)), sizeof(SectionHeader));

	return SUCCESS;
}

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
	static EepromStore & getInstance() { static EepromStore _instance; return _instance; }

	Status loadSection(char *name, void *buf, int *length);
	Status storeSection(char *name, void *buf, int length);
	Status deleteSection(char *name);

private:

	class SectionEntry : public ListNode {
	public:
		SectionEntry *next() { return (SectionEntry *)_next; }
		bool equals(ListNode *other);
		SectionHeader header;				// The header info stored in EEPROM
		int			  offset;				// The offset in EEPROM where data is stored
	};

	LinkedList	_occupied;					// The linked list of occupied sections
	LinkedList  _free;						// The linked list of free sections
	EepromStore();
	EepromStore(EepromStore const&);        // Don't Implement to disallow copy by assignment
    void operator=(EepromStore const&);		// Don't implement to disallow copy by assignment
	void init(void);
	SectionEntry *findSection(const char *name);
};

#endif /* MEGABOTICS_EEPROMSTRORE_H_ */

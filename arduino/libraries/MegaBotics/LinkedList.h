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
 * LinkedList.h
 *
 *  Created on: Apr 20, 2015
 *      Author: Aakash Sahai
 */

#ifndef MEGABOTICS_LINKEDLIST_H_
#define MEGABOTICS_LINKEDLIST_H_

#include <stdlib.h>

class ListNode {
public:
	virtual bool equals(ListNode *other) = 0;

protected:
	friend class LinkedList;
	ListNode *	_next;
};

class LinkedList {
public:
	LinkedList() { _head = NULL; _tail = NULL; _current = NULL; _size = 0; }
	virtual ~LinkedList();


	ListNode * insert(ListNode *node);	// Add to tail
	ListNode * remove(ListNode *node);	// Remove from list

	bool isEmpty() 	{ return _head == NULL; }
	ListNode *first() { _current = _head; return _head; }
	ListNode *next(ListNode *node) {
		if (_current == NULL)
			return NULL;
		_current = _current->_next;
		return _current;
	}
private:
	ListNode	* _head;
	ListNode	* _tail;
	ListNode	* _current;
	int 		_size;
};

#endif /* MEGABOTICS_LINKEDLIST_H_ */

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
 * LinkedList.cpp
 *
 *  Created on: Apr 20, 2015
 *      Author: Aakash Sahai
 */

#include "LinkedList.h"

LinkedList::~LinkedList() {

}

ListNode * LinkedList::insert(ListNode *node) {
	  node->_next = NULL;
	  if (_tail)
		  _tail->_next = node;
	  _tail = node;
	  if (isEmpty())
	    _head = _tail;
	  _size++;
	  return node;
}

ListNode * LinkedList::remove(ListNode *node) {
	if (isEmpty())			// Take care of empty list
		return NULL;

	if (node == _head) {	// Take care of special case
		_head = _head->_next;
		if (_tail == node) {
			_tail = _head;
		}
		node->_next = NULL;
		return node;
	}

	ListNode *curr = _head->_next;
	ListNode *prev = _head;
	while (curr != NULL) {
		if (curr == node) {
			prev->_next = curr->_next;
			if (_tail == node) {
				_tail = prev;
			}
			node->_next = NULL;
			return node;
		}
		prev = curr;
		curr = curr->_next;
	}

	return NULL;
}

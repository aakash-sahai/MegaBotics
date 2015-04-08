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
 * CircularBuffer.h
 *
 *  Created on: Apr 7, 2015
 *      Author: Aakash Sahai
 */

#ifndef MEGABOTICS_CIRCULARBUFFER_H_
#define MEGABOTICS_CIRCULARBUFFER_H_

#include <stdlib.h>

template <class T>
class CircularBuffer {
public:
	CircularBuffer() { _head = 0; _tail = 0; _maxElements = 0; _buffer = 0; }
	CircularBuffer(int maxElements) { _head = 0; _tail = 0; _maxElements = maxElements + 1; _buffer = (T *)malloc(sizeof(T) * _maxElements); }
	virtual ~CircularBuffer() { if (_buffer) free((void*) _buffer); }

	// Number of elements that can be written
	void setup(int len)   { _maxElements = len + 1; _buffer = (T *)malloc(sizeof(T) * _maxElements); }
	int capacity() { return (_maxElements - 1 - _head + _tail) % _maxElements ; }
	bool isFull()	{ return nextHead() == _tail; }
	bool isEmpty() 	{ return _head == _tail; }
	bool peekStack(T *data);	// Peek at data @ head
	bool peekQueue(T *data);	// Peek at data @ tail
	bool enqueue(const T data);	// Queue at head
	bool dequeue(T *data);		// Dequeue from tail
	bool push(T data);			// Push at head
	bool pop(T *data);			// Pop from head
	int nextHead() { return (_head + 1 ) % _maxElements; }
	int nextTail() { return (_tail + 1 ) % _maxElements; }
private:
	int _head;
	int _tail;
	int _maxElements;
	T *_buffer;
};

template <class T>
bool CircularBuffer<T>::enqueue(const T data) {
	int head;
	if ((head = nextHead()) == _tail)
		return false;
	else
		_head = head;
	_buffer[_head] = data;
	return true;
}

template <class T>
bool CircularBuffer<T>::dequeue(T *data) {
	if (isEmpty())
		return false;
	_tail = nextTail();
	*data = _buffer[_tail];
	return true;
}

template <class T>
bool CircularBuffer<T>::peekQueue(T *data) {
	if (isEmpty())
		return false;
	*data = _buffer[nextTail()];
	return true;
}

template <class T>
bool CircularBuffer<T>::push(T data) {
	int head;
	if ((head = nextHead()) == _tail)
		return false;
	else
		_head = head;
	_buffer[_head] = data;
	return true;
}

template <class T>
bool CircularBuffer<T>::pop(T *data) {
	if (isEmpty())
		return false;
	*data = _buffer[_head];
	if (--_head < 0) _head = _maxElements - 1;
	return true;
}

template <class T>
bool CircularBuffer<T>::peekStack(T *data) {
	if (isEmpty())
		return false;
	*data = _buffer[_head];
	return true;
}

#endif /* MEGABOTICS_CIRCULARBUFFER_H_ */

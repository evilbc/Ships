#include "LinkedList.h"
#include "Ships.h"
#include <iostream>
#include <assert.h>
#include "Constants.h"

PlaneLinkedList::PlaneLinkedList() {
	head = NULL;
	tail = NULL;
	size = 0;
}

PlaneLinkedList::~PlaneLinkedList() {
	clear();
}

Node* PlaneLinkedList::createNode(SpyPlane* value) {
	Node* newNode = new Node();
	newNode->next = NULL;
	newNode->previous = NULL;
	newNode->value = value;
	return newNode;
}
void PlaneLinkedList::push(SpyPlane* value) {
	Node* last = tail;
	tail = createNode(value);
	tail->previous = last;
	if (head == NULL) {
		head = tail;
	} else {
		last->next = tail;
	}
	size++;
}
int PlaneLinkedList::length() {
	return size;
}
SpyPlane* PlaneLinkedList::get(const int index) {
	assert(index < size);
	if (index + 1 == size) {
		return peek();
	}
	Node* current = head;
	for (int i = 0; i < index; i++) {
		current = current->next;
	}
	return current->value;
}

SpyPlane* PlaneLinkedList::pop() {
	assert(size != 0);
	SpyPlane* value = tail->value;
	if (size == 1) {
		delete head;
		head = NULL;
		tail = NULL;
	} else {
		Node* newLast = tail->previous;
		newLast->next = NULL;
		delete tail;
		tail = newLast;
	}
	size--;
	return value;
}

SpyPlane* PlaneLinkedList::peek() {
	assert(size != 0);
	return tail->value;
}

bool PlaneLinkedList::isEmpty() {
	return size == 0;
}

void PlaneLinkedList::addFirst(SpyPlane* value) {
	Node* newHead = createNode(value);
	newHead->next = head;
	head->previous = newHead;
	head = newHead;
	size++;
}
void PlaneLinkedList::removeFirst() {
	if (size <= 1) {
		pop();
		return;
	}
	Node* newHead = head->next;
	head = newHead;
	delete newHead->previous;
	newHead->previous = NULL;
	size--;
}

void PlaneLinkedList::clear() {
	Node* current = tail;
	if (tail == NULL) {
		return;
	}
	while (current->previous != NULL) {
		current = current->previous;
		delete current->next;
	}
	delete current;
	head = NULL;
	tail = NULL;
	size = 0;
}
#include "PointerArrayList.h"
#include <assert.h>
#include "Constants.h"
#include "Ships.h"

ShipPointerArrayList::ShipPointerArrayList(const int initialSize) {
	values = new Ship*[initialSize];
	size = 0;
	maxSize = initialSize;
	for (int i = 0; i < initialSize; i++) {
		values[i] = NULL;
	}
}

void ShipPointerArrayList::add(Ship* value, const int index) {
	if (index > maxSize) {
		maxSize = index;
		Ship** newValues = new Ship*[maxSize];
		for (int i = 0; i < size; i++) {
			newValues[i] = values[i];
		}
		delete[] values;
		values = newValues;
	}
	values[index] = value;
	size++;
}
int ShipPointerArrayList::length() {
	return size;
}
Ship* ShipPointerArrayList::get(const int index) {
	assert(index < maxSize);
	if (index >= size) {
		return NULL;
	}
	return values[index];
}
void ShipPointerArrayList::remove(const int index) {
	assert(size > 0);
	if (values[index] == NULL) return;
	delete values[index];
	values[index] = NULL;
	size--;
}
bool ShipPointerArrayList::isEmpty() {
	return size == 0;
}
bool ShipPointerArrayList::containsIndex(const int index) {
	return values[index] != NULL;
}
ShipPointerArrayList::~ShipPointerArrayList() {
	for (int i = 0; i < maxSize; i++) {
		delete values[i];
	}
	delete[] values;
}
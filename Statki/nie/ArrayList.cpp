#include "ArrayList.h"
#include <stdexcept>

template <class T>
void ArrayList<T>::add(const T value) {
	if (length == maxLength) {
		maxLength *= 1.5;
		T newValues[] = new T[maxLength];
		for (int i = 0; i < length; i++) {
			newValues[i] = values[i];
		}
		delete[] values;
		values = newValues;
	}
	values[length++] = value;
}

template <class T>
int ArrayList<T>::size() {
	return length;
}

template <class T>
T ArrayList<T>::get(const int index) {
	if (index >= length) {
		throw std::invalid_argument("Index is higher than list size");
	}
	return values[index];
}

template <class T>
void ArrayList<T>::remove(const int index) {
	if (index >= this->length) {
		throw std::invalid_argument("Index is higher than list size");
	}
	moveToLeft(index + 1);
	length--;
}

template <class T>
bool ArrayList<T>::isEmpty() {
	return length == 0;
}

template <class T>
bool ArrayList<T>::contains(const T value) {
	for (int i = 0; i < length; i++) {
		if (values[i] == value) {
			return true;
		}
	}
	return false;
}

template <class T>
void ArrayList<T>::deleteList() {
	delete[] values;
	delete this;
}

template <class T>
void ArrayList<T>::moveToLeft(const int start) {
	for (int i = start; i < length; i++) {
		values[i - 1] = values[i];
	}
}
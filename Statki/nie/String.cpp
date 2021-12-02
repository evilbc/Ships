#include "String.h"
#include <iostream>

String::String(const char* value) {
	length = strlen(value);
	this->value = new char[length + 1];
	copy(value, this->value);
	this->value[length] = '\0';
}


String::~String() {
	delete[] value;
}

String String::getInput(const int maxInputSize) {
	char* buffer = new char[maxInputSize];
	std::cin >> buffer;
	String result = String(buffer);
	delete[] buffer;
	return result;
}

String String::operator+(const char* value) {
	int newLength = length + strlen(value);
	char* newVal = new char[newLength + 1];
	copy(this->value, newVal);
	delete[] this->value;
	int j = 0;
	for (int i = length; i < newLength; i++) {
		newVal[i] = value[i++];
	}
	newVal[newLength] = '\0';
	this->value = newVal;
	length = newLength;
	return *this;
}

String String::operator+(const char value) {
	char* newVal = new char[length + 2];
	copy(this->value, newVal);
	delete[] this->value;
	newVal[length] = value;
	newVal[length + 1] = '\0';
	this->value = newVal;
	length += 1;
	return *this;
}

bool String::operator== (const char* value) {
	if (length != strlen(value)) return false;
	for (int i = 0; i < length; i++) {
		if (this->value[i] != value[i]) return false;
	}
	return true;
}
bool String::operator== (const char value) {
	if (length != 1 || this->value[0] != value) return false;
	return true;
}
bool String::operator== (const String value) {
	return operator==(value.value);
}


void String::copy(const char* from, char* to) {
	for (unsigned int i = 0; i < strlen(from) + 1; i++) {
		to[i] = from[i];
	}
}
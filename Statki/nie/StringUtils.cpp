char* concat(const char* string1, const char* string2) {
	int string1Length = strLength(string1);
	int length = string1Length + strLength(string2) + 1;
	char* result = new char[length];
	copy(string1, result);
}

char* concat(const char* string, const char ch) {

}

char* concat(const char ch, const char* string) {

}

void copy(const char* from, char* to) {
	for (int i = 0; i < strLength(from); i++) {
		to[i] = from[i];
	}
}

int strLength(const char* string) {
	for (int i = 0;; i++) {
		if (string[i] == '\0') return i;
	}
}
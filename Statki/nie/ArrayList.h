#ifndef ARRAY_LIST
#define ARRAY_LIST
template <class T>
class ArrayList {
private:
	T values[];
	int length;
	int maxLength;
	void moveToLeft(const int start) {
		for (int i = start; i < length; i++) {
			values[i - 1] = values[i];
		}
	}
public:
	ArrayList<T>(const int initialSize) {
		values = new T[initialSize];
		length = 0;
		maxLength = initialSize;
	}
	ArrayList<T>() {
		this(10);
	}
	void add(const T value) {
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
	int size() {
		return length;
	}
	T get(const int index) {
		if (index >= length) {
			throw std::invalid_argument("Index is higher than list size");
		}
		return values[index];
	}
	void remove(const int index) {
		if (length == 0) {
			throw std::invalid_argument("List is empty");
		}
		if (index >= length) {
			throw std::invalid_argument("Index is higher than list size");
		}
		moveToLeft(index + 1);
		length--;
	}
	bool isEmpty() {
		return length == 0;
	}
	bool contains(const T value) {
		for (int i = 0; i < length; i++) {
			if (values[i] == value) {
				return true;
			}
		}
		return false;
	}
	void deleteList() {
		delete[] values;
		delete this;
	}
};
#endif

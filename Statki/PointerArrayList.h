#ifndef SHIP_POINTER_ARRAY_LIST
#define SHIP_POINTER_ARRAY_LIST

class Ship;

class ShipPointerArrayList {
private:
	Ship** values;
	int size;
	int maxSize;
public:
	ShipPointerArrayList(const int initialSize = 10);
	~ShipPointerArrayList();
	void add(Ship* value, const int index);
	int length();
	Ship* get(const int index);
	void remove(const int index);
	bool isEmpty();
	bool containsIndex(const int index);
};
#endif
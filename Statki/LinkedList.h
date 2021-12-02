#ifndef LINKED_LIST
#define LINKED_LIST

struct SpyPlane;
struct Node {
	SpyPlane* value;

	Node* next;
	Node* previous;
};


class PlaneLinkedList {
private:
	Node* head;
	Node* tail;
	int size;
	Node* createNode(SpyPlane* value);

public:
	PlaneLinkedList();
	~PlaneLinkedList();
	void push(SpyPlane* value);
	int length();
	SpyPlane* get(const int index);
	SpyPlane* pop();
	SpyPlane* peek();
	void addFirst(SpyPlane* value);
	void removeFirst();
	bool isEmpty();
};
#endif
#ifndef LINKED_LIST
#define LINKED_LIST

#include <stdexcept>

template <class T>
struct Node {
	T value;

	Node<T>* next;
	Node<T>* previous;
};

template <class T>
class LinkedList {
private:
	Node<T>* head;
	Node<T>* tail;
	int length;
	Node<T>* createNode(const T value) {
		Node<T>* newNode = new Node<T>();
		newNode->next = nullptr;
		newNode->previous = nullptr;
		newNode->value = value;
		return newNode;
	}

public:
	LinkedList<T>() {
		head = nullptr;
		tail = nullptr;
		length = 0;
	}
	void add(const T value) {
		Node<T>* last = tail;
		tail = createNode(value);
		tail->previous = last;
		if (head == nullptr) {
			head = tail;
		} else {
			last->next = tail;
		}
		length++;
	}
	int size() {
		return length;
	}
	T get(const int index) {
		if (index >= length) {
			throw std::invalid_argument("Index is higher than list size");
		}
		if (index + 1 == length) {
			return peek();
		}
		Node<T>* current = head;
		for (int i = 0; i < index; i++) {
			current = current->next;
		}
		return current->value;
	}
	void remove(const int index) {
		if (index >= length) {
			throw std::invalid_argument("Index is higher than list size");
		}

		if (length == 1 || index + 1 == length) {
			pop();
			return;
		}
		if (index == 0) {
			removeFirst();
		}

		Node<T>* currentNode = head;
		for (int i = 0; i < index; i++) {
			currentNode = currentNode->next;
		}
		currentNode->previous = currentNode->next;
		currentNode->next = currentNode->previous;
		delete currentNode;
		length--;
	}
	T pop() {
		if (length == 0) {
			throw std::invalid_argument("List is empty");
		}
		T value = tail->value;
		if (length == 1) {
			delete head;
			head = nullptr;
			tail = nullptr;
			return value;
		}
		Node<T>* newLast = tail->previous;
		newLast->next = nullptr;
		delete tail;
		tail = newLast;
		length--;
		return value;
	}
	T peek() {
		if (length == 0) {
			throw std::invalid_argument("List is empty");
		}
		return tail->value;
	}
	bool isEmpty() {
		return length == 0;
	}
	bool contains(const T value) {
		Node<T>* current = head;
		while (current != nullptr) {
			if (current->value == value) {
				return true;
			}
		}
		return false;
	}
	void addFirst(const T value) {
		Node<T>* newHead = createNode(value);
		newHead->next = head;
		head->previous = newHead;
		head = newHead;
		length++;
	}
	void removeFirst() {
		if (length <= 1) {
			pop();
			return;
		}
		Node<T>* newHead = head->next;
		head = newHead;
		delete newHead->previous;
		newHead->previous = nullptr;
		length--;
	}
	void deleteList() {
		Node<T>* current = tail;
		while (current->previous != nullptr) {
			delete current->next;
		}
		delete current;
		delete this;
	}
};
#endif
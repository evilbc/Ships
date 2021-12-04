#ifndef PRIORITY_QUEUE
#define PRIORITY_QUEUE
struct ShootingTarget {
	double targetValue;
	bool isSpying;
	int x, y;
};


class ShootingTargetPriorityQueue {
private:
	int size, maxSize, firstIndex;
	ShootingTarget* values;
	void moveToRight(const int index);
public:
	ShootingTargetPriorityQueue(int initialSize = 100);
	~ShootingTargetPriorityQueue();
	bool isFull();
	bool isEmpty();
	void add(ShootingTarget value);
	ShootingTarget remove();
	void clear();
};

int modulo(const int num, const int mod);
#endif
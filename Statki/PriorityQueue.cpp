#include "PriorityQueue.h"
#include <assert.h>

ShootingTargetPriorityQueue::ShootingTargetPriorityQueue(int initialSize) {
	values = new ShootingTarget[initialSize];
	size = firstIndex = 0;
	maxSize = initialSize;
}

ShootingTargetPriorityQueue::~ShootingTargetPriorityQueue(){
	delete[] values;
}

bool ShootingTargetPriorityQueue::isFull() {
	return size >= maxSize;
}

bool ShootingTargetPriorityQueue::isEmpty() {
	return size == 0;
}

void ShootingTargetPriorityQueue::add(ShootingTarget value) {
	assert(size < maxSize);
	int index = firstIndex;
	while (index < size) {
		if (value.targetValue > values[index].targetValue) {
			moveToRight(index);
			break;
		}
		index = (index + 1) % maxSize;
	}
	values[index] = value;
	size++;
}

ShootingTarget ShootingTargetPriorityQueue::remove() {
	ShootingTarget result = values[firstIndex];
	firstIndex = (firstIndex + 1) % maxSize;
	size--;
	return result;
}

void ShootingTargetPriorityQueue::moveToRight(const int index) {
	int firstEmptyIndex = (firstIndex + size) % maxSize;
	int iterCount = modulo((firstEmptyIndex - index), maxSize);
	while (iterCount > 0) {
		int prevIndex = modulo(firstEmptyIndex - 1, maxSize);
		values[firstEmptyIndex] = values[prevIndex];
		iterCount--;
		firstEmptyIndex = prevIndex;
	}
}

int modulo(const int num, const int mod) {
	return (mod + (num % mod)) % mod;
}

void ShootingTargetPriorityQueue::clear() {
	size = firstIndex = 0;
}
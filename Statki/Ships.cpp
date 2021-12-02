#include "Ships.h"
#include <assert.h>
#include "Constants.h"
#include <string.h>
#include <iostream>
#include "Command.h"
#include "Enums.h"
#include "LinkedList.h"


Ship::Ship(const int _length, const ShipTypes _type, const int _maxMovesInRound)
	: length(_length), type(_type), maxMovesInRound(_maxMovesInRound), maxShotsInRound(_length) {
	roundOfLastMove = lastRoundMoveCount = x = y = roundOfLastShot = lastRoundShotCount = cannonX = cannonY = UNINITIALIZED_INT;
	positions = new ShipPosition * [length];
	initPositions();
	direction = Directions::INVALID;
	totalShotsMade = 0;
}

Ship::~Ship() {
	for (int i = 0; i < length; i++) {
		delete positions[i];
	}
	delete[] positions;
}

bool Ship::shotHit(const int position) {
	if (positions[position]->sign == SIGN_SHIP_DESTROYED) {
		return false;
	}
	positions[position]->sign = SIGN_SHIP_DESTROYED;
	return true;
}

void Ship::initPositions() {
	for (int i = 0; i < length; i++) {
		positions[i] = new ShipPosition();
		positions[i]->sign = SIGN_SHIP_PRESENT_DEFAULT;
		if (i == SHIP_RADAR_POSITION) {
			positions[i]->sign = SIGN_SHIP_PRESENT_RADAR;
			continue;
		}
		if (i == SHIP_CANNON_POSITION) {
			positions[i]->sign = SIGN_SHIP_PRESENT_CANNON;
		}
		if (i == SHIP_ENGINE_POSITION) {
			positions[i]->sign = SIGN_SHIP_PRESENT_ENGINE;
		}
	}
}

void Ship::save(const char playerName, const int index) {
	char* shipPositions = new char[length + 1];
	for (int i = 0; i < length; i++) {
		shipPositions[i] = ((positions[i]->isDestroyed()) ? OP_SHIP_DESTROYED_FIELD : OP_SHIP_PRESENT_FIELD);
	}
	shipPositions[length] = '\0';
	printf("%s %c %d %d %c %d %s %s\n", OPERATION_SHIP, playerName, y, x, charFromDirection(direction), index, charArrFromShipType(type), shipPositions);
	delete[] shipPositions;
}

char Ship::getCharToPrint(const int position) {
	return positions[position]->sign;
}

bool Ship::canSee(int _x, int _y) {
	int xChange, yChange;
	getXAndYChangeFromDirection(direction, &xChange, &yChange);
	int newX = x;
	int newY = y;
	for (int i = 0; i < length; i++) {
		if (_x == x && _y == y) {
			return true;
		}
		newX += xChange;
		newY += yChange;
	}
	return isInRadarRange(_x, _y);
}

bool Ship::isInRadarRange(int _x, int _y) {
	if (positions[SHIP_RADAR_POSITION]->isDestroyed()) {
		return _x >= x - 1 && _x <= x + 1 && _y >= y - 1 && _y <= y + 1;
	}
	return distance(_x, _y, x, y) <= length;
}

bool Ship::canShoot(ShootCmd* cmd) {
	if (positions[SHIP_CANNON_POSITION]->isDestroyed()) {
		cmd->setErrorMsg("SHIP CANNOT SHOOT");
		return false;
	}
	if (roundOfLastShot == cmd->roundNum && lastRoundShotCount >= maxShotsInRound) {
		cmd->setErrorMsg("TOO MANY SHOOTS");
		return false;
	}
	getCannonXAndY();
	if (!isInShootingRange(cannonX, cannonY)) {
		cmd->setErrorMsg("SHOOTING TOO FAR");
		return false;
	}
	updateShotCount(cmd->roundNum);
	roundOfLastShot = cmd->roundNum;
	return true;
}

bool Ship::isInShootingRange(int _x, int _y) {
	return distance(_x, _y, x, y) <= length;
}

bool Carrier::isInShootingRange(int x, int y) {
	return true;
}


bool Ship::canMove(MoveCmd* cmd) {
	if (positions[SHIP_ENGINE_POSITION]->isDestroyed()) {
		cmd->setErrorMsg("SHIP CANNOT MOVE");
		return false;
	}
	if (roundOfLastMove == cmd->roundNum && lastRoundMoveCount >= maxMovesInRound) {
		cmd->setErrorMsg("SHIP MOVED ALREADY");
		return false;
	}
	return true;
}

void Ship::moved(MoveCmd* cmd) {
	if (roundOfLastMove == cmd->roundNum) {
		lastRoundMoveCount++;
	} else {
		lastRoundMoveCount = 1;
	}
	roundOfLastMove = cmd->roundNum;
}

Battleship::Battleship() : Ship(LENGTH_BATTLESHIP, ShipTypes::BATTLESHIP) {}
Cruiser::Cruiser() : Ship(LENGTH_CRUISER, ShipTypes::CRUISER) {}
Destroyer::Destroyer() : Ship(LENGTH_DESTROYER, ShipTypes::DESTROYER) {}
Carrier::Carrier() : Ship(LENGTH_CARRIER, ShipTypes::CARRIER, MAX_MOVES_CARRIER) {
	planes = new PlaneLinkedList();
}
Carrier::~Carrier() {
	delete planes;
}

void Carrier::spy(SpyCmd* cmd) {
	if (positions[SHIP_CANNON_POSITION]->isDestroyed()) {
		cmd->setErrorMsg("CANNOT SEND PLANE");
	}
	if (roundOfLastShot == cmd->roundNum && lastRoundShotCount >= maxShotsInRound) {
		cmd->setErrorMsg("ALL PLANES SENT");
		return;
	}
	updateShotCount(cmd->roundNum);
	roundOfLastShot = cmd->roundNum;
}

void Ship::updateShotCount(const int cmdRoundNum) {
	if (roundOfLastShot == cmdRoundNum) {
		lastRoundShotCount++;
	} else {
		lastRoundShotCount = 1;
	}
}

bool ShipPosition::isDestroyed() {
	return sign == SIGN_SHIP_DESTROYED;
}


bool SpyPlane::canSee(int _x, int _y) {
	return (_x <= x + 1 && _x >= x - 1 && _y <= y + 1 && _y >= y + 1);
}


double Ship::distance(int x1, int y1, int x2, int y2) {
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void Ship::getCannonXAndY() {
	int xChange, yChange;
	getXAndYChangeFromDirection(direction, &xChange, &yChange);
	cannonX = x + (xChange * SHIP_CANNON_POSITION);
	cannonY = y + (yChange * SHIP_CANNON_POSITION);
}

bool Carrier::planesCanSee(int _x, int _y) {
	for (int i = 0; i < planes->length(); i++) {
		if (planes->get(i)->canSee(_x, _y)) {
			return true;
		}
	}
	return false;
}
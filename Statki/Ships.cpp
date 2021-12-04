#include "Ships.h"
#include <assert.h>
#include "Constants.h"
#include <string.h>
#include <iostream>
#include "Command.h"
#include "Enums.h"
#include "LinkedList.h"
#include "Board.h"


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
	if (cmd->isAuto) {
		if (positions[SHIP_CANNON_POSITION]->isDestroyed()) {
			return false;
		}
		AutoShootArgs args;
		args.shotsLeft = (roundOfLastShot == cmd->roundNum)
			? maxShotsInRound - lastRoundShotCount
			: maxShotsInRound;
		args.movesLeft = (roundOfLastMove == cmd->roundNum)
			? maxMovesInRound - lastRoundMoveCount
			: maxMovesInRound;
		args.x = x;
		args.y = y;
		args.direction = direction;
		args.shootX = cmd->x;
		args.shootY = cmd->y;
		args.len = length;
		args.board = cmd->board;
		bool result  = canAutoShoot(args);
		if (result) {
			updateShotCount(cmd->roundNum);
			roundOfLastShot = cmd->roundNum;
		}
		return result;
	}
	if (positions[SHIP_CANNON_POSITION]->isDestroyed()) {
		cmd->setErrorMsg("SHIP CANNOT SHOOT");
		return false;
	}
	if (roundOfLastShot == cmd->roundNum && lastRoundShotCount >= maxShotsInRound) {
		cmd->setErrorMsg("TOO MANY SHOOTS");
		return false;
	}
	if (!isInShootingRange(cmd->x, cmd->y)) {
		cmd->setErrorMsg("SHOOTING TOO FAR");
		return false;
	}
	updateShotCount(cmd->roundNum);
	roundOfLastShot = cmd->roundNum;
	return true;
}

bool Ship::canAutoShoot(AutoShootArgs args) {
	if (args.direction == Directions::INVALID) {
		return false;
	}
	if (args.shotsLeft == 0) {
		return false;
	}
	if (shotFeasible(&args)) {
		return true;
	}
	if (args.movesLeft == 0 || positions[SHIP_ENGINE_POSITION]->isDestroyed()) {
		return false;
	}
	AutoShootArgs argsF = args;
	AutoShootArgs argsL = args;
	AutoShootArgs argsR = args;
	simulateMove(&argsF, MoveDir::FORWARD);
	simulateMove(&argsL, MoveDir::LEFT);
	simulateMove(&argsR, MoveDir::RIGHT);
	return canAutoShoot(argsF) || canAutoShoot(argsL) || canAutoShoot(argsR);
}


bool Ship::shotFeasible(AutoShootArgs* args) {
	if (type == ShipTypes::CARRIER) {
		return true;
	}
	getCannonXAndY(args);
	return distance(args->cannonX, args->cannonY, args->shootX, args->shootY) <= length;
}

bool Ship::isInShootingRange(int _x, int _y) {
	getCannonXAndY();
	return distance(cannonX, cannonY, _x, _y) <= length;
}

bool Carrier::isInShootingRange(int _x, int _y) {
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

Battleship::Battleship() : Ship(LENGTH_BATTLESHIP, ShipTypes::BATTLESHIP) {
}
Cruiser::Cruiser() : Ship(LENGTH_CRUISER, ShipTypes::CRUISER) {
}
Destroyer::Destroyer() : Ship(LENGTH_DESTROYER, ShipTypes::DESTROYER) {
}
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
	sendPlane(cmd);
}

bool Carrier::canSpy(SpyCmd* cmd) {
	if (positions[SHIP_CANNON_POSITION]->isDestroyed()) {
		return false;
	}
	if (roundOfLastShot == cmd->roundNum && lastRoundShotCount >= maxShotsInRound) {
		return false;
	}
	sendPlane(cmd);
	return true;
}

void Carrier::sendPlane(SpyCmd* cmd) {
	updateShotCount(cmd->roundNum);
	roundOfLastShot = cmd->roundNum;
	SpyPlane* plane = new SpyPlane();
	plane->x = cmd->x;
	plane->y = cmd->y;
	planes->push(plane);
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
	return (_x <= x + 1 && _x >= x - 1 && _y <= y + 1 && _y >= y - 1);
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

void Ship::getCannonXAndY(AutoShootArgs* args) {
	int xChange, yChange;
	getXAndYChangeFromDirection(args->direction, &xChange, &yChange);
	args->cannonX = args->x + (xChange * SHIP_CANNON_POSITION);
	args->cannonY = args->y + (yChange * SHIP_CANNON_POSITION);
}

bool Carrier::planesCanSee(int _x, int _y) {
	for (int i = 0; i < planes->length(); i++) {
		if (planes->get(i)->canSee(_x, _y)) {
			return true;
		}
	}
	return false;
}

void Ship::simulateMove(AutoShootArgs* args, MoveDir dir) {
	int xChange, yChange;
	int oldX = args->x;
	int oldY = args->y;
	getXAndYChangeFromDirection(args->direction, &xChange, &yChange);
	switch (dir) {
	case MoveDir::FORWARD:
		args->direction = args->direction;
		args->x = oldX - xChange;
		args->y = oldY - yChange;
		break;
	case MoveDir::RIGHT:
		args->direction = (Directions)(((int)args->direction + 1) % 4);
		if (xChange != 0) {
			args->x = oldX - xChange;
			args->y = oldY - (xChange * (length - 1));
		} else {
			args->y = oldY - yChange;
			args->x = oldX + (yChange * (length - 1));
		}
		break;
	case MoveDir::LEFT:
		args->direction = (Directions)(((int)args->direction + 3) % 4);
		if (xChange != 0) {
			args->x = oldX - xChange;
			args->y = oldY + (xChange * (length - 1));
			break;
		} else {
			args->y = oldY - yChange;
			args->x = oldX - (yChange * (length - 1));
		}
		break;
	}
	args->movesLeft--;
	if (args->board->isPositionValid(args->x, args->y, NULL, args->direction, this) != NULL) {
		args->direction = Directions::INVALID;
	}
}
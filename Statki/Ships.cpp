#include "Ships.h"
#include <assert.h>
#include "Constants.h"
#include <string.h>
#include <iostream>
#include "Command.h"
#include "Enums.h"
#include "LinkedList.h"
#include "Board.h"


Ship::Ship(const int length, const ShipTypes type, const char playerName, const int maxMovesInRound)
	: length(length), type(type), maxMovesInRound(maxMovesInRound), maxShotsInRound(length), playerName(playerName) {
	roundOfLastMove = lastRoundMoveCount = x = y = roundOfLastShot
		= lastRoundShotCount = cannonX = cannonY = simulatedX = simulatedY = UNINITIALIZED_INT;
	positions = new ShipPosition * [length];
	initPositions();
	direction = simulatedDirection = Directions::INVALID;
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

void Ship::save(const int index) {
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

bool Ship::canSee(int x, int y, bool isSimulated) {
	int xChange, yChange;
	getXAndYChangeFromDirection(direction, &xChange, &yChange);
	int newX = this->x;
	int newY = this->y;
	if (isSimulated) {
		newX = simulatedX;
		newY = simulatedY;
	}
	for (int i = 0; i < length; i++) {
		if (x == newX && y == newY) {
			return true;
		}
		newX += xChange;
		newY += yChange;
	}
	return isInRadarRange(x, y, isSimulated);
}

bool Ship::isInRadarRange(int x, int y, bool isSimulated) {
	int thisX = this->x;
	int thisY = this->y;
	if (isSimulated) {
		thisX = simulatedX;
		thisY = simulatedY;
	}
	if (positions[SHIP_RADAR_POSITION]->isDestroyed()) {
		return x >= thisX - 1 && x <= thisX + 1 && y >= thisY - 1 && y <= thisY + 1;
	}
	return distance(x, y, thisX, thisY) <= length;
}

bool Ship::canShoot(ShootCmd* cmd) {
	if (cmd->isAuto) {
		return canAutoShoot(cmd);
	}
	if (isCannonDestroyed()) {
		cmd->setErrorMsg("SHIP CANNOT SHOOT");
		return false;
	}
	if (roundOfLastShot == cmd->roundNum && lastRoundShotCount >= maxShotsInRound) {
		cmd->setErrorMsg("TOO MANY SHOOTS");
		return false;
	}
	if (!isInShootingRange(cmd->x, cmd->y, cmd->isSimulated)) {
		cmd->setErrorMsg("SHOOTING TOO FAR");
		return false;
	}
	if (cmd->isSimulated) {
		return true;
	}
	updateShotCount(cmd->roundNum);
	roundOfLastShot = cmd->roundNum;
	return true;
}

bool Ship::canAutoShoot(ShootCmd* cmd) {
	if (isCannonDestroyed()) {
		return false;
	}
	AutoShootArgs args;
	args.shotsLeft = shotsLeft(cmd->roundNum);
	args.movesLeft = movesLeft(cmd->roundNum);
	args.x = x;
	args.y = y;
	args.direction = direction;
	args.shootX = cmd->x;
	args.shootY = cmd->y;
	args.len = length;
	args.board = cmd->board;
	bool result = canAutoShoot(args);
	if (result && !cmd->isSimulated) {
		updateShotCount(cmd->roundNum);
		roundOfLastShot = cmd->roundNum;
	}
	return result;
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
	if (args.movesLeft == 0 || isCannonDestroyed()) {
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

bool Ship::isInShootingRange(int x, int y, bool isSimulated) {
	getCannonXAndY(isSimulated);
	return distance(cannonX, cannonY, x, y) <= length;
}

bool Carrier::isInShootingRange(int x, int y, bool isSimulated) {
	return true;
}


bool Ship::canMove(MoveCmd* cmd) {
	if (isEngineDestroyed()) {
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
	if (cmd->isSimulated) {
		return;
	}
	if (roundOfLastMove == cmd->roundNum) {
		lastRoundMoveCount++;
	} else {
		lastRoundMoveCount = 1;
	}
	roundOfLastMove = cmd->roundNum;
}

bool Ship::isEngineDestroyed() {
	return positions[SHIP_ENGINE_POSITION]->isDestroyed();
}

bool Ship::isCannonDestroyed() {
	return positions[SHIP_CANNON_POSITION]->isDestroyed();
}

int Ship::shotsLeft(const int roundNum) {
	return (roundOfLastShot == roundNum)
		? maxShotsInRound - lastRoundShotCount
		: maxShotsInRound;
}

int Ship::movesLeft(const int roundNum) {
	return (roundOfLastMove == roundNum)
		? maxMovesInRound - lastRoundMoveCount
		: maxMovesInRound;
}

Battleship::Battleship(const char playerName) : Ship(LENGTH_BATTLESHIP, ShipTypes::BATTLESHIP, playerName) {
}
Cruiser::Cruiser(const char playerName) : Ship(LENGTH_CRUISER, ShipTypes::CRUISER, playerName) {
}
Destroyer::Destroyer(const char playerName) : Ship(LENGTH_DESTROYER, ShipTypes::DESTROYER, playerName) {
}
Carrier::Carrier(const char playerName) : Ship(LENGTH_CARRIER, ShipTypes::CARRIER, playerName, MAX_MOVES_CARRIER) {
	planes = new PlaneLinkedList();
	simulatedPlanes = new PlaneLinkedList();
}
Carrier::~Carrier() {
	delete planes;
	delete simulatedPlanes;
}

void Carrier::spy(SpyCmd* cmd) {
	if (isCannonDestroyed()) {
		cmd->setErrorMsg("CANNOT SEND PLANE");
	}
	if (roundOfLastShot == cmd->roundNum && lastRoundShotCount >= maxShotsInRound) {
		cmd->setErrorMsg("ALL PLANES SENT");
		return;
	}
	sendPlane(cmd);
}

bool Carrier::canSpy(SpyCmd* cmd) {
	if (isCannonDestroyed()) {
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

void Carrier::simSpy(const int x, const int y) {
	SpyPlane* plane = new SpyPlane();
	plane->x = x;
	plane->y = y;
	simulatedPlanes->push(plane);
}

void Carrier::clearSimulatedPlanes() {
	simulatedPlanes->clear();
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


bool SpyPlane::canSee(int x, int y) {
	return (x <= this->x + 1 && x >= this->x - 1 && y <= this->y + 1 && y >= this->y - 1);
}


double Ship::distance(int x1, int y1, int x2, int y2) {
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void Ship::getCannonXAndY(bool isSimulated) {
	int xChange, yChange;
	Directions dir = direction;
	int thisX = x;
	int thisY = y;
	if (isSimulated) {
		thisX = simulatedX;
		thisY = simulatedY;
		dir = simulatedDirection;
	}
	getXAndYChangeFromDirection(dir, &xChange, &yChange);
	cannonX = thisX + (xChange * SHIP_CANNON_POSITION);
	cannonY = thisY + (yChange * SHIP_CANNON_POSITION);
}

void Ship::getCannonXAndY(AutoShootArgs* args) {
	int xChange, yChange;
	getXAndYChangeFromDirection(args->direction, &xChange, &yChange);
	args->cannonX = args->x + (xChange * SHIP_CANNON_POSITION);
	args->cannonY = args->y + (yChange * SHIP_CANNON_POSITION);
}

bool Carrier::planesCanSee(int x, int y, bool isSimulated) {
	for (int i = 0; i < planes->length(); i++) {
		if (planes->get(i)->canSee(x, y)) {
			return true;
		}
	}
	if (isSimulated) {
		for (int i = 0; i < simulatedPlanes->length(); i++) {
			if (simulatedPlanes->get(i)->canSee(x, y)) {
				return true;
			}
		}
	}
	return false;
}

void Ship::simulateMove(AutoShootArgs* args, MoveDir dir) {
	MoveParams* params = new MoveParams(args->x, args->y, length, args->direction, dir);
	args->direction = params->direction;
	args->x = params->x;
	args->y = params->y;
	args->movesLeft--;
	if (args->board->isPositionValid(args->x, args->y, NULL, args->direction, this) != NULL) {
		args->direction = Directions::INVALID;
	}
}
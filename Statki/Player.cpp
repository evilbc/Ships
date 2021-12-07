#include "Player.h"
#include <iostream>
#include "PointerArrayList.h"
#include "Ships.h"
#include "Game.h"
#include "Board.h"
#include <assert.h>
#include "Command.h"
#include "Enums.h"
#include "PriorityQueue.h"
using namespace std;

Player::Player(Board* board, char playerName) {
	carriers = new ShipPointerArrayList();
	battleships = new ShipPointerArrayList();
	cruisers = new ShipPointerArrayList();
	destroyers = new ShipPointerArrayList();
	setFleet();
	this->board = board;
	this->playerName = playerName;
	isAi = false;
	piecesLeft = 0;
	aiAllShipsArePlaced = false;
	lastRoundOfAi = UNINITIALIZED_INT;
}

Player::~Player() {
	delete carriers;
	delete battleships;
	delete cruisers;
	delete destroyers;
}

void Player::placeShip(PlaceShipCmd* cmd) {
	setShip(cmd);
}

Ship* Player::setShip(ShipCreatingCmd* cmd) {
	ShipPointerArrayList* shipList = getListOfShipType((cmd->getShipType()));
	if (shipList->containsIndex(cmd->shipIndex)) {
		cmd->setErrorMsg("SHIP ALREADY PRESENT");
		return NULL;
	}
	if (getMaxNumberOfShipType(cmd->getShipType()) <= shipList->length()) {
		cmd->setErrorMsg("ALL SHIPS OF THE CLASS ALREADY SET");
		return NULL;
	}
	Ship* ship = createShip(cmd->getShipType());
	cmd->ship = ship;
	cmd->playerName = playerName;
	if (board->placeShip(cmd)) {
		piecesLeft += ship->length;
		shipList->add(ship, cmd->shipIndex);
		return ship;
	}
	return NULL;
}

Ship* Player::createShip(const ShipTypes shipType) {
	switch (shipType) {
	case ShipTypes::CARRIER:
		return new Carrier(playerName);
	case ShipTypes::BATTLESHIP:
		return new Battleship(playerName);
	case ShipTypes::CRUISER:
		return new Cruiser(playerName);
	case ShipTypes::DESTROYER:
		return new Destroyer(playerName);
	}
	assert(!"Incorrect ship type");
	return NULL;
}

void Player::setFleet(const int maxCarriers, const int maxBattleships, const int maxCruisers, const int maxDestroyers) {
	maxCarrierNum = maxCarriers;
	maxBattleshipNum = maxBattleships;
	maxCruiserNum = maxCruisers;
	maxDestroyerNum = maxDestroyers;
}

int Player::getMaxNumberOfShipType(const ShipTypes shipType) {
	switch (shipType) {
	case ShipTypes::CARRIER:
		return maxCarrierNum;
	case ShipTypes::CRUISER:
		return maxCruiserNum;
	case ShipTypes::BATTLESHIP:
		return maxBattleshipNum;
	case ShipTypes::DESTROYER:
		return maxDestroyerNum;
	}
	assert(!"Incorrect ship type");
	return 0;
}

ShipPointerArrayList* Player::getListOfShipType(const ShipTypes shipType) {
	switch (shipType) {
	case ShipTypes::CARRIER:
		return carriers;
	case ShipTypes::CRUISER:
		return cruisers;
	case ShipTypes::BATTLESHIP:
		return battleships;
	case ShipTypes::DESTROYER:
		return destroyers;
	}
	assert(!"Incorrect ship type");
	return NULL;
}

bool Player::allShipsPlaced() {
	return (maxCarrierNum == carriers->length() && maxBattleshipNum == battleships->length()
		&& maxCruiserNum == cruisers->length() && maxDestroyerNum == destroyers->length());
}

void Player::beShot(ShootCmd* cmd) {
	if (board->shotHit(cmd, playerName)) {
		piecesLeft--;
	}
}

int Player::getPiecesLeft() {
	return piecesLeft;
}

void Player::ship(ShipCmd* cmd) {
	Ship* createdShip = setShip(cmd);
	if (createdShip == NULL) {
		return;
	}
	for (unsigned int i = 0; i < strlen(cmd->shipFields); i++) {
		if (cmd->shipFields[i] == OP_SHIP_DESTROYED_FIELD) {
			createdShip->shotHit(i);
			piecesLeft--;
		}
	}
}

void Player::moveShip(MoveCmd* cmd) {
	ShipPointerArrayList* shipList = getListOfShipType(cmd->getShipType());
	cmd->ship = shipList->get(cmd->shipIndex);
	board->moveShip(cmd);
}

const char Player::getPlayerName() {
	return playerName;
}

void Player::saveInitialPositions() {
	board->saveInitPositions(playerName);
	printf("%s %c %d %d %d %d\n", OPERATION_SET_FLEET, playerName, maxCarrierNum, maxBattleshipNum, maxCruiserNum, maxDestroyerNum);
	saveInitialPositions(carriers);
	saveInitialPositions(battleships);
	saveInitialPositions(cruisers);
	saveInitialPositions(destroyers);
}

void Player::saveInitialPositions(ShipPointerArrayList* list) {
	for (int i = 0; i < list->length(); i++) {
		Ship* ship = list->get(i);
		assert(ship != NULL);
		if (ship == NULL) {
			continue;
		}
		ship->save(i);
	}
}

void Player::spy(SpyCmd* cmd) {
	if (!cmd->isAuto) {
		Carrier* carrier = (Carrier*)carriers->get(cmd->shipIndex);
		assert(carrier != NULL);
		carrier->spy(cmd);
		return;
	};
	for (int i = 0; i < carriers->length(); i++) {
		if (((Carrier*)carriers->get(cmd->shipIndex))->canSpy(cmd)) {
			return;
		}
	}
	cmd->setErrorMsg("SPYING IS UNFEASIBLE");
	return;
}

bool Player::canSee(int x, int y, bool isSimulated) {
	if (shipCanSee(x, y, carriers, isSimulated)
		|| shipCanSee(x, y, battleships, isSimulated)
		|| shipCanSee(x, y, cruisers, isSimulated)
		|| shipCanSee(x, y, destroyers, isSimulated)) {
		return true;
	}
	for (int i = 0; i < carriers->length(); i++) {
		if (((Carrier*)carriers->get(i))->planesCanSee(x, y, isSimulated)) {
			return true;
		}
	}
	return false;
}

bool Player::shipCanSee(int x, int y, ShipPointerArrayList* list, bool isSimulated) {
	for (int i = 0; i < list->length(); i++) {
		if (list->get(i)->canSee(x, y, isSimulated)) {
			return true;
		}
	}
	return false;
}

bool Player::canShoot(ShootCmd* cmd) {
	if (cmd->isAuto) {
		if (canShoot(cmd, carriers)
			|| canShoot(cmd, battleships)
			|| canShoot(cmd, cruisers)
			|| canShoot(cmd, destroyers)) {
			return true;
		} else {
			cmd->setErrorMsg("SHOOTS ARE UNFEASIBLE");
			return false;
		}
	}
	ShipPointerArrayList* list = getListOfShipType(cmd->getShipType());
	Ship* ship = list->get(cmd->shipIndex);
	assert(ship != NULL);
	return ship->canShoot(cmd);

}

bool Player::canShoot(ShootCmd* cmd, ShipPointerArrayList* list) {
	for (int i = 0; i < list->length(); i++) {
		if (list->get(i)->canShoot(cmd)) {
			return true;
		}
	}
	return false;
}

bool Player::aiShouldExit(const int roundNum) {
	if (lastRoundOfAi == roundNum) {
		return true;
	}
	lastRoundOfAi = roundNum;
	return false;
}

void Player::aiPlaceShips(const int roundNum) {
	if (aiShouldExit(roundNum)) {
		return;
	}
	aiPrintState();
	printf("[player%c]\n", playerName);
	Board* simulatedBoard = new Board(*board);
	ShipPointerArrayList* toDelete[NUMBER_OF_SHIP_TYPES];
	toDelete[0] = aiPlaceShips(ShipTypes::CARRIER, simulatedBoard);
	toDelete[1] = aiPlaceShips(ShipTypes::BATTLESHIP, simulatedBoard);
	toDelete[2] = aiPlaceShips(ShipTypes::CRUISER, simulatedBoard);
	toDelete[3] = aiPlaceShips(ShipTypes::DESTROYER, simulatedBoard);
	cleanUpMockShips(toDelete);
	delete simulatedBoard;
	aiAllShipsArePlaced = true;
	printf("[player%c]\n", playerName);
	aiPrintState();
}

void Player::handleAi(const int roundNum) {
	if (aiShouldExit(roundNum)) {
		return;
	}
	aiPrintState();
	printf("[player%c]\n", playerName);
	Board* simulatedBoard = new Board(*board);
	AiMoveParams* params = new AiMoveParams();
	params->roundNum = roundNum;
	params->simulatedBoard = simulatedBoard;
	params->queue = new ShootingTargetPriorityQueue();
	aiMove(battleships, params);
	aiMove(cruisers, params);
	aiMove(destroyers, params);
	aiMove(carriers, params);
	printf("[player%c]\n", playerName);
	aiPrintState();
	clearSimulatedPlanes();
	delete params->queue;
	delete params;
	delete simulatedBoard;
}

void Player::aiPrintState() {
	printf("\n%s\n%s %d\n%s\n", TOGGLE_STATE_COMMANDS, OPERATION_PRINT, BASIC_PRINT, TOGGLE_STATE_COMMANDS);
}

void Player::clearSimulatedPlanes() {
	for (int i = 0; i < carriers->length(); i++) {
		((Carrier*)carriers->get(i))->clearSimulatedPlanes();
	}
}

void Player::aiMove(ShipPointerArrayList* list, AiMoveParams* params) {
	for (int i = 0; i < list->length(); i++) {
		params->ship = list->get(i);
		params->shipIndex = i;
		aiMove(params);
		params->queue->clear();
	}
}


void Player::aiMove(AiMoveParams* params) {
	params->madeAllMoves = false;
	Ship* ship = params->ship;
	params->shotsLeft = ship->maxShotsInRound;
	int maxMovesToMake = ship->maxMovesInRound;
	int triesLeft = AI_MAX_TRIES;
	if (ship->isEngineDestroyed()) {
		params->madeAllMoves = true;
		tryToShoot(params);
		return;
	}
	tryToShoot(params);
	while (maxMovesToMake > 0 && triesLeft > 0) {
		MoveDir dir = getRandomMoveDir();
		MoveCmd* cmd = new MoveCmd(params->roundNum, dir, playerName, ship);
		if (params->simulatedBoard->moveShip(cmd)) {
			maxMovesToMake--;
			printf("%s %d %s %c\n", OPERATION_MOVE, params->shipIndex, charArrFromShipType(ship->type), static_cast<char>(dir));
			tryToShoot(params);
		}
		triesLeft--;
		delete cmd;
	}
	params->madeAllMoves = true;
	tryToShoot(params);
}


ShipPointerArrayList* Player::aiPlaceShips(ShipTypes type, Board* simulatedBoard) {
	int max = getMaxNumberOfShipType(type);
	ShipPointerArrayList* list = getListOfShipType(type);
	int shipsLeftToPlace = max - list->length();
	Directions direction;
	bool isPlaced = false;
	int y, x;
	ShipPointerArrayList* toDelete = new ShipPointerArrayList();
	for (int i = 0; i < max && shipsLeftToPlace > 0; i++) {
		if (list->containsIndex(i)) {
			continue;
		}
		Ship* ship = createShip(type);
		isPlaced = false;
		while (!isPlaced) {
			y = rand() % simulatedBoard->getHeight();
			x = rand() % simulatedBoard->getWidth();
			direction = getRandomDirection();
			ShipCreatingCmd* cmd = new ShipCreatingCmd(x, y, direction, playerName, type, ship);
			isPlaced = simulatedBoard->placeShip(cmd);
			delete cmd;
		}
		toDelete->add(ship, --shipsLeftToPlace);
		printf("%s %d %d %c %d %s\n", OPERATION_PLACE_SHIP, y, x, charFromDirection(direction), i, charArrFromShipType(type));
	}
	return toDelete;
}
void Player::cleanUpMockShips(ShipPointerArrayList* toDelete[NUMBER_OF_SHIP_TYPES]) {
	for (int k = 0; k < NUMBER_OF_SHIP_TYPES; k++) {
		delete toDelete[k];
	}
}

void Player::tryToShoot(AiMoveParams* params) {
	Ship* ship = params->ship;
	if (ship->isCannonDestroyed() || params->shotsLeft <= 0) {
		return;
	}
	if (ship->type == ShipTypes::CARRIER && params->madeAllMoves) {
		tryToSpy(params);
	}
	findBestShots(params);
	if (params->madeAllMoves) {
		findRandomShots(params);
	}
	while (params->shotsLeft > 0 && !params->queue->isEmpty()) {
		ShootingTarget bestTarget = params->queue->remove();
		if (!board->isWithinBounds(bestTarget.x, bestTarget.y)) {
			continue;
		}
		if (params->simulatedBoard->simAlreadyShot(bestTarget.x, bestTarget.y)) {
			continue;
		}
		if (!bestTarget.isSpying) {
			params->simulatedBoard->simulateHit(bestTarget.x, bestTarget.y);
			printf("%s %d %s %d %d\n", OPERATION_SHOOT, params->shipIndex, charArrFromShipType(ship->type), bestTarget.y, bestTarget.x);
		} else {
			((Carrier*)ship)->simSpy(bestTarget.x, bestTarget.y);
			printf("%s %d %d %d\n", OPERATION_SPY, params->shipIndex, bestTarget.y, bestTarget.x);
		}
		params->shotsLeft--;
	}
}

void Player::findBestShots(AiMoveParams* params) {
	Ship* ship = params->ship;
	AiShootingRange* range = new AiShootingRange(ship, params->simulatedBoard);
	for (int i = range->minXRange; i <= range->maxXRange; i++) {
		for (int j = range->minYRange; j <= range->maxYRange; j++) {
			if (params->simulatedBoard->isWithinBounds(i, j) && canSee(i, j, true) && params->simulatedBoard->isEnemyShip(i, j, playerName)) {
				ShootCmd* cmd = new ShootCmd(i, j, params->roundNum, ship->type, params->shipIndex);
				if (canShoot(cmd)) {
					params->queue->add(getShootingTarget(i, j, params->simulatedBoard));
				}
				delete cmd;
			}
		}
	}
	delete range;
}

ShootingTarget Player::getShootingTarget(const int x, const int y, Board* simBoard, bool canSee) {
	ShootingTarget target;
	target.x = x;
	target.y = y;
	if (canSee) {
		target.targetValue = calculateValueOfShot(simBoard->getShipAtPosition(x, y), simBoard->getPositionOfShip(x, y));
	} else {
		target.targetValue = BLIND_SHOT_VALUE;
	}
	target.isSpying = false;
	return target;
}

double Player::calculateValueOfShot(Ship* ship, const int position) {
	double shipTypeMultiplier = pow(ship->length, 2) * LENGTH_MULTIPLIER;
	double positionMultiplier = BASE_MULTIPLIER;
	const char sign = ship->getCharToPrint(position);
	if (sign == SIGN_SHIP_PRESENT_CANNON) {
		positionMultiplier = CANNON_MULTIPLIER;
	} else if (sign == SIGN_SHIP_PRESENT_ENGINE) {
		positionMultiplier = ENGINE_MULTIPLIER;
	} else if (sign == SIGN_SHIP_PRESENT_RADAR) {
		positionMultiplier = RADAR_MULTIPLIER;
	}
	return shipTypeMultiplier * positionMultiplier;
}

void Player::tryToSpy(AiMoveParams* params) {
	int triesLeft = AI_MAX_TRIES;
	int planesSent = 0;
	while (planesSent < 5 && triesLeft > 0) {
		int x = rand() % params->simulatedBoard->getWidth();
		int y = rand() & params->simulatedBoard->getHeight();
		if (params->simulatedBoard->isWithinBounds(x, y) && !canSee(x, y, true)) {
			ShootingTarget target;
			target.x = x;
			target.y = y;
			target.targetValue = SPYING_VALUE;
			target.isSpying = true;
			params->queue->add(target);
		}
		triesLeft--;
	}
}


void Player::findRandomShots(AiMoveParams* params) {
	Ship* ship = params->ship;
	AiShootingRange* range = new AiShootingRange(ship, board);
	int x, y;
	int triesLeft = AI_MAX_TRIES;
	while (triesLeft > 0) {
		x = range->minXRange + (rand() % range->maxXRange - range->minXRange);
		y = range->minYRange + (rand() % range->maxYRange - range->minYRange);
		if (params->simulatedBoard->isWithinBounds(x, y), !canSee(x, y, true) && !params->simulatedBoard->simAlreadyShot(x, y)) {
			ShootCmd* cmd = new ShootCmd(x, y, params->roundNum, ship->type, params->shipIndex);
			if (canShoot(cmd)) {
				params->queue->add(getShootingTarget(x, y, params->simulatedBoard, false));
			}
			delete cmd;
		}
		triesLeft--;
	}
	delete range;
}


AiShootingRange::AiShootingRange(Ship* ship, Board* simBoard) {
	len = ship->length;
	if (ship->type == ShipTypes::CARRIER) {
		minXRange = minYRange = 0;
		maxXRange = simBoard->getWidth();
		maxYRange = simBoard->getHeight();
		return;
	}
	minXRange = ship->simulatedX - len;
	maxXRange = ship->simulatedX + len;
	minYRange = ship->simulatedY - len;
	maxYRange = ship->simulatedY + len;
}
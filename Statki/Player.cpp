#include "Player.h"
#include <iostream>
#include "PointerArrayList.h"
#include "Ships.h"
#include "Game.h"
#include "Board.h"
#include <assert.h>
#include "Command.h"
#include "Enums.h"
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
		return new Carrier();
	case ShipTypes::BATTLESHIP:
		return new Battleship();
	case ShipTypes::CRUISER:
		return new Cruiser();
	case ShipTypes::DESTROYER:
		return new Destroyer();
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
		ship->save(playerName, i);
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

bool Player::canSee(int x, int y) {
	if (shipCanSee(x, y, carriers)
		|| shipCanSee(x, y, battleships)
		|| shipCanSee(x, y, cruisers)
		|| shipCanSee(x, y, destroyers)) {
		return true;
	}
	for (int i = 0; i < carriers->length(); i++) {
		if (((Carrier*)carriers->get(i))->planesCanSee(x, y)) {
			return true;
		}
	}
	return false;
}

bool Player::shipCanSee(int x, int y, ShipPointerArrayList* list) {
	for (int i = 0; i < list->length(); i++) {
		if (list->get(i)->canSee(x, y)) {
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

void Player::handleAi() {
	cout << "START AI GENERATED COMMANDS" << endl;
	if (!aiAllShipsArePlaced) {

	}
	aiMove(carriers);
	aiMove(battleships);
	aiMove(cruisers);
	aiMove(destroyers);
	cout << "END AI GENERATED COMMANDS, COPY AND PASTE THEM TO EXECUTE THEM" << endl;
}

void Player::aiMove(ShipPointerArrayList* list) {
	for (int i = 0; i < list->length(); i++) {
		//SimulationResult result;
		//result = getBestMove(result);
		//result.simulatedBoard->performMoves(result);
		//cout << result.commands;
	}
}

void Player::aiPlaceShips() {
	Board* simulatedBoard = new Board(board);
	aiPlaceShips(ShipTypes::CARRIER, simulatedBoard);
	aiPlaceShips(ShipTypes::BATTLESHIP, simulatedBoard);
	aiPlaceShips(ShipTypes::CRUISER, simulatedBoard);
	aiPlaceShips(ShipTypes::DESTROYER, simulatedBoard);
	delete simulatedBoard;
	aiAllShipsArePlaced = true;
}

void Player::aiPlaceShips(ShipTypes type, Board* simulatedBoard) {
	int max = getMaxNumberOfShipType(type);
	ShipPointerArrayList* list = getListOfShipType(type);
	int shipsLeftToPlace = max - list->length();
	Directions direction;
	bool isPlaced = false;
	int y, x;

	for (int i = 0; i < max && shipsLeftToPlace > 0; i++) {
		if (list->containsIndex(i)) {
			continue;
		}
		isPlaced = false;;
		while (!isPlaced) {
			y = rand() % simulatedBoard->getHeight();
			x = rand() % simulatedBoard->getWidth();
			direction = getRandomDirection();
			isPlaced = simulatedBoard->aiCanPlace();
		}
		printf("%s %d %d %c %d %s", OPERATION_PLACE_SHIP, y, x, charFromDirection(direction), i, charArrFromShipType(type));
	}
}
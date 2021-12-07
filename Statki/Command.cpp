#include "Command.h"
#include "Constants.h"
#include <iostream>
#include <assert.h>
#include "Ships.h"
#include "Game.h"
#include "Enums.h"
#include "Board.h"


Command::Command(const char* args, const char* opName) {
	fullOp = new char[MAX_INPUT_LENGTH];
	strcpy(fullOp, opName);
	strcat(fullOp, args);
	errorMsg = NULL;
}

char* Command::getFullOp() {
	return fullOp;
}

Command::~Command() {
	delete[] fullOp;
	if (errorMsg != NULL) {
		delete[] errorMsg;
	}
}

char* Command::getErrorMsg() {
	return errorMsg;
}

void Command::setErrorMsg(const char* msg) {
	if (errorMsg != NULL) {
		assert(!"Error message already exists");
		return;
	}
	errorMsg = new char[MAX_INPUT_LENGTH];
	errorMsg = strcpy(errorMsg, msg);
}

bool Command::isError() {
	return errorMsg != NULL;
}

XAndYCmd::XAndYCmd() {
	x = y = UNINITIALIZED_INT;
}

ShipCreatingCmd::ShipCreatingCmd(char* args, const char* opName) : Command(args, opName) {
	shipIndex = x = y = UNINITIALIZED_INT;
	direction = playerName = NULL;
	shipType = new char[MAX_INPUT_LENGTH];
	ship = NULL;
	isSimulated = false;
}

ShipCreatingCmd::ShipCreatingCmd(const int x, const int y, const Directions direction, const char playerName, const ShipTypes shipType, Ship* ship) : Command() {
	shipIndex = UNINITIALIZED_INT;
	this->direction = charFromDirection(direction);
	this->x = x;
	this->y = y;
	this->shipType = new char[MAX_INPUT_SHIP_TYPE_LENGTH];
	strcpy(this->shipType, charArrFromShipType(shipType));
	this->playerName = playerName;
	this->ship = ship;
	isSimulated = true;
}

ShipTypes ShipCreatingCmd::getShipType() {
	return shipTypeFromAbbreviation(shipType);
}

Directions ShipCreatingCmd::getDirection() {
	return directionFromChar(direction);
}

ShipCreatingCmd::~ShipCreatingCmd() {
	if (shipType != NULL) {
		delete[] shipType;
	}
}

PlaceShipCmd::PlaceShipCmd(char* args) : ShipCreatingCmd(args, OPERATION_PLACE_SHIP) {
}

ShipCmd::ShipCmd(char* args) : ShipCreatingCmd(args, OPERATION_SHIP) {
	shipFields = new char[MAX_INPUT_SHIP_SIZE_LENGTH];
}

ShipCmd::~ShipCmd() {
	delete[] shipFields;
}

SpyCmd::SpyCmd(char* args) : Command(args, OPERATION_SPY) {
	shipIndex = x = y = roundNum = UNINITIALIZED_INT;
	isAuto = false;
}

PrintCmd::PrintCmd(char* args) : Command(args, OPERATION_PRINT) {
	type = UNINITIALIZED_INT;
	player = NULL;
	isSimulated = false;
}

PrintCmd::PrintCmd(int type, Player* player) : Command() {
	this->type = type;
	this->player = player;
	isSimulated = true;
}

bool PrintCmd::isExtended() {
	assert(type == BASIC_PRINT || type == 1);
	return type != BASIC_PRINT;
}

bool PrintCmd::isPlayer() {
	return player != NULL;
}

SetFleetCmd::SetFleetCmd(char* args) : Command(args, OPERATION_SET_FLEET) {
	numOfCarriers = numOfBattleships = numOfCruisers = numOfDestroyers = UNINITIALIZED_INT;
	playerName = NULL;
}

NextPlayerCmd::NextPlayerCmd(char* args) : Command(args, OPERATION_NEXT_PLAYER) {
	playerName = NULL;
}

BoardSizeCmd::BoardSizeCmd(char* args) : Command(args, OPERATION_BOARD_SIZE) {
	height = width = UNINITIALIZED_INT;
}

InitPositionsCmd::InitPositionsCmd(char* args) : Command(args, OPERATION_INIT_POSITION) {
	playerName = NULL;
	y1 = y2 = x1 = x2 = UNINITIALIZED_INT;
}

ExtendedShips::ExtendedShips(char* args) : Command(args, OPERATION_EXTENDED_SHIPS) {
}

ReefCmd::ReefCmd(char* args) : Command(args, OPERATION_REEF) {
}

SetAiPlayerCmd::SetAiPlayerCmd(char* args) : Command(args, OPERATION_SET_AI_PLAYER) {
	playerName = NULL;
}

SrandCmd::SrandCmd(char* args) : Command(args, OPERATION_SRAND) {
	value = UNINITIALIZED_INT;
}

SaveCmd::SaveCmd(char* args) : Command(args, OPERATION_SAVE) {
}

AutoShootingCmd::AutoShootingCmd(char* args) : Command(args, OPERATION_AUTO_SHOOTING) {
}

InformationCmd::InformationCmd(char* args) : Command(args, OPERATION_INFORMATION) {
	playerName = NULL;
}

ShootCmd::ShootCmd(char* args) : Command(args, OPERATION_SHOOT) {
	roundNum = x = y = shipIndex = UNINITIALIZED_INT;
	shipType = new char[MAX_INPUT_SHIP_TYPE_LENGTH];
	extendedLogic = isAuto = isSimulated = false;
	board = NULL;
}

ShootCmd::ShootCmd(const int x, const int y, const int roundNum, const ShipTypes shipType, const int index) : Command() {
	this->x = x;
	this->y = y;
	this->roundNum = roundNum;
	shipIndex = index;
	this->shipType = new char[MAX_INPUT_SHIP_TYPE_LENGTH];
	strcpy(this->shipType, charArrFromShipType(shipType));
	extendedLogic = isAuto = false;
	isSimulated = true;
	board = NULL;
}

ShootCmd::~ShootCmd() {
	delete[] shipType;
}

ShipTypes ShootCmd::getShipType() {
	return shipTypeFromAbbreviation(shipType);
}

MoveCmd::MoveCmd(char* args) : Command(args, OPERATION_MOVE) {
	shipIndex = roundNum = newX = newY = UNINITIALIZED_INT;
	dir = NULL;
	shipType = new char[MAX_INPUT_SHIP_TYPE_LENGTH];
	ship = NULL;
	newDirection = Directions::INVALID;
	playerName = NULL;
	isSimulated = false;
}

MoveCmd::MoveCmd(const int roundNum, const MoveDir dir, const char playerName, Ship* ship, const bool isSimulated) : Command() {
	shipIndex = newX = newY = UNINITIALIZED_INT;
	this->roundNum = roundNum;
	this->dir = static_cast<char>(dir);
	this->playerName = playerName;
	this->shipType = new char[MAX_INPUT_SHIP_TYPE_LENGTH];
	strcpy(this->shipType, charArrFromShipType(ship->type));
	newDirection = Directions::INVALID;
	this->ship = ship;
	this->isSimulated = isSimulated;
}

MoveCmd::~MoveCmd() {
	delete[] shipType;
}

MoveDir MoveCmd::getMoveDir() {
	return moveDirFormChar(dir);
}

ShipTypes MoveCmd::getShipType() {
	return shipTypeFromAbbreviation(shipType);
}
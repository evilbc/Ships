#include "Command.h"
#include "Constants.h"
#include <iostream>
#include <assert.h>
#include "Ships.h"
#include "Game.h"
#include "Enums.h"


Command::Command(char* args, const char* opName) {
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

//ShipPlacingCmd::ShipPlacingCmd(char* args, const char* opName) : Command(args, opName) {}

//ShipPlacingCmd::ShipPlacingCmd(char* args, const char* opName) : Command(args, opName) { }
//ShipPlacingCmd::~ShipPlacingCmd() {
//	assert(!"TEST");
//}

ShipCreatingCmd::ShipCreatingCmd(char* args, const char* opName) : Command(args, opName) {
	shipIndex = x = y = UNINITIALIZED_INT;
	direction = playerName = NULL;
	shipType = new char[MAX_INPUT_LENGTH];
	ship = NULL;
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
	playerName = NULL;
}

ShipCmd::~ShipCmd() {
	delete[] shipFields;
}

SpyCmd::SpyCmd(char* args) : Command(args, OPERATION_SPY) {
	shipIndex = x = y = roundNum = UNINITIALIZED_INT;
}

PrintCmd::PrintCmd(char* args) : Command(args, OPERATION_PRINT) {
	type = UNINITIALIZED_INT;
	player = NULL;
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
	roundNum = shipIndex = UNINITIALIZED_INT;
	shipType = new char[MAX_INPUT_LENGTH];
	extendedLogic = false;
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
	shipType = new char[MAX_INPUT_LENGTH];
	ship = NULL;
	newDirection = Directions::INVALID;
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
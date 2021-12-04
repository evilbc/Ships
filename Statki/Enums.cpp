#include "Enums.h"
#include <assert.h>
#include <string.h>
#include "Constants.h"
#include <iostream>

ShipTypes shipTypeFromAbbreviation(char input[]) {
	if (strcmp(input, CARRIER_ABBR) == 0) return ShipTypes::CARRIER;
	if (strcmp(input, BATTLESHIP_ABBR) == 0) return ShipTypes::BATTLESHIP;
	if (strcmp(input, CRUISER_ABBR) == 0) return ShipTypes::CRUISER;
	if (strcmp(input, DESTROYER_ABBR) == 0) return ShipTypes::DESTROYER;
	assert(!"No such ship type");
	return ShipTypes::INVALID;
}


void getXAndYChangeFromDirection(const Directions direction, int* xChange, int* yChange) {
	switch (direction) {
	case Directions::UP:
		*xChange = 0;
		*yChange = 1;
		return;
	case Directions::DOWN:
		*xChange = 0;
		*yChange = -1;
		return;
	case Directions::RIGHT:
		*xChange = -1;
		*yChange = 0;
		return;
	case Directions::LEFT:
		*xChange = 1;
		*yChange = 0;
		return;
	}
}

const char charFromDirection(Directions direction) {
	switch (direction) {
	case Directions::UP:
		return DIRECTION_UP;
	case Directions::DOWN:
		return DIRECTION_DOWN;
	case Directions::LEFT:
		return DIRECTION_LEFT;
	case Directions::RIGHT:
		return DIRECTION_RIGHT;
	default:
		assert(!"Invalid direction");
		return NULL;
	}
}
const char* charArrFromShipType(ShipTypes type) {
	switch (type) {
	case ShipTypes::CARRIER:
		return CARRIER_ABBR;
	case ShipTypes::BATTLESHIP:
		return BATTLESHIP_ABBR;
	case ShipTypes::CRUISER:
		return CRUISER_ABBR;
	case ShipTypes::DESTROYER:
		return DESTROYER_ABBR;
	default:
		assert(!"Invalid type");
		return "INVALID";
	}
}

Directions getRandomDirection() {
	return static_cast<Directions>(rand() % 4);
}

Directions directionFromChar(const char ch) {
	if (ch == DIRECTION_UP) return Directions::UP;
	if (ch == DIRECTION_DOWN) return Directions::DOWN;
	if (ch == DIRECTION_RIGHT) return Directions::RIGHT;
	if (ch == DIRECTION_LEFT) return Directions::LEFT;
	assert(!"No such direction");
	return Directions::INVALID;
}

MoveDir moveDirFormChar(const char ch) {
	//if (ch == 'F') return MoveDir::FORWARD;
	//if (ch == 'L') return MoveDir::LEFT;
	//if (ch == 'R') return MoveDir::RIGHT;
	//assert(!"No such move dir");
	//return MoveDir::INVALID;
	return static_cast<MoveDir>(ch);
}
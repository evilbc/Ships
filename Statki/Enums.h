#ifndef ENUMS
#define ENUM
enum class ShipTypes {
	CARRIER,
	BATTLESHIP,
	CRUISER,
	DESTROYER,
	INVALID
};

ShipTypes shipTypeFromAbbreviation(char input[]);

enum class Directions {
	UP = 0,
	RIGHT = 1,
	DOWN = 2,
	LEFT = 3,
	INVALID
};
void getXAndYChangeFromDirection(Directions direction, int* xChange, int* yChange);
Directions directionFromChar(const char ch);
const char charFromDirection(Directions direction);
const char* charArrFromShipType(ShipTypes type);

enum class MoveDir {
	FORWARD = 'F',
	LEFT = 'L',
	RIGHT = 'R',
	INVALID
};
MoveDir moveDirFormChar(const char ch);
#endif
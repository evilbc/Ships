#ifndef ENUMS
#define ENUM
enum class ShipTypes {
	CARRIER,
	BATTLESHIP,
	CRUISER,
	DESTROYER,
	INVALID
};

enum class MoveDir {
	FORWARD = 'F',
	LEFT = 'L',
	RIGHT = 'R',
	INVALID
};

enum class Directions {
	UP = 0,
	RIGHT = 1,
	DOWN = 2,
	LEFT = 3,
	INVALID
};
ShipTypes shipTypeFromAbbreviation(char input[]);

void getXAndYChangeFromDirection(Directions direction, int* xChange, int* yChange);
Directions directionFromChar(const char ch);
Directions getRandomDirection();
const char charFromDirection(Directions direction);
const char* charArrFromShipType(ShipTypes type);
MoveDir getRandomMoveDir();
MoveDir moveDirFormChar(const char ch);
#endif
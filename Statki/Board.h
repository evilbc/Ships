#ifndef BOARD
#define BOARD
#define _CRT_SECURE_NO_WARNINGS
#include "Constants.h"
class Ship;
class ShipCreatingCmd;
class InitPositionsCmd;
class ReefCmd;
class BoardSizeCmd;
class ShootCmd;
class PrintCmd;
class PlaceShipCmd;
class MoveCmd;
class Command;
enum class Directions;

struct Point {
	int x, y;
	bool isReef;
	Ship* occupyingShip;
	char occupyingPlayer;
	int shipPosition;
	void print(PrintCmd* cmd, const bool extendedLogic);
	void setOccupyingPlayer(const char name);
};

class Board {
private:
	Point*** points;
	int heightDigitCount, widthDigitCount, boardHeight, boardWidth;
	bool tooCloseToOtherShip(int x, int y, Ship* ship);
	void setShipInPosition(int x, int y, Directions direction, Ship* ship, const char playerName, const bool isSimulated);
	void setShipInPosition(MoveCmd* cmd);
	void setShipInPosition(ShipCreatingCmd* cmd);
	bool isPositionValid(MoveCmd* cmd);
	bool isPositionValid(ShipCreatingCmd* cmd);
	bool isOccupiedByOtherShip(int x, int y, Ship* ship);
	void printWidthIndices();
	void printHeightIndices(int row);
	void deletePoints();
	bool isWithinBounds(int x, int y);
	void setMoveParameters(MoveCmd* cmd);
	void unsetShipFromOldPosition(MoveCmd* cmd);
	char playerOfPosition(const int x, const int y);
public:
	Board();
	Board(const Board& obj);
	~Board();
	bool placeShip(ShipCreatingCmd* cmd);
	int getHeight();
	int getWidth();
	void print(PrintCmd* cmd);
	void addShip(ShipCreatingCmd* cmd);
	bool shotHit(ShootCmd* cmd, const char shotPlayerName);
	void setSize(const Board& obj);
	void setSize(const int height = BOARD_HEIGHT_DEFAULT, const int width = BOARD_WIDTH_DEFAULT);
	void setSize(BoardSizeCmd* cmd);
	void setReef(ReefCmd* cmd);
	void initPositions(InitPositionsCmd* cmd);
	bool extendedLogic;
	void moveShip(MoveCmd* cmd);
	void saveReefs();
	void saveInitPositions(const char playerName);
	bool isOccupiedByPlayer(const int x, const int y, const char playerName);
	const char* isPositionValid(int x, int y, const char playerName, Directions direction, Ship* ship);
};

int digitCount(int number);
int digitAt(const int index, int number);
#endif
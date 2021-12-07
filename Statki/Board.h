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
enum class MoveDir;

struct Point {
	int x, y;
	bool isReef;
	Ship* occupyingShip;
	char occupyingPlayer;
	int shipPosition;
	void print(PrintCmd* cmd, const bool extendedPrinting, const bool isSimulated = false);
	void setOccupyingPlayer(const char name);
	bool simulationAlreadyShot;
	bool simulationDestroyed;
};

struct MoveParams {
	int x, y;
	Directions direction;
	MoveParams(const int x, const int y, const int length, const Directions dir, const MoveDir moveDir);
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
	void setMoveParameters(MoveCmd* cmd);
	void unsetShipFromOldPosition(MoveCmd* cmd);
	char playerOfPosition(const int x, const int y);
	bool wontBeAbleToMakeAnotherMove(const int x, const int y, const Directions direction);
	bool isFacingEdge(const int x, const int y, const Directions direction);
	bool isFacingReef(const int x, const int y, const Directions direction);
public:
	Board();
	Board(const Board& obj);
	~Board();
	bool isWithinBounds(int x, int y);
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
	void initPositions(const InitPositionsCmd* cmd);
	bool extendedLogic;
	bool moveShip(MoveCmd* cmd);
	void saveReefs();
	void saveInitPositions(const char playerName);
	bool isOccupiedByPlayer(const int x, const int y, const char playerName);
	const char* isPositionValid(int x, int y, const char playerName, Directions direction, Ship* ship, bool isSimulated = false);
	void simulateHit(const int x, const int y);
	bool isEnemyShip(const int x, const int y, const char playerName);
	Ship* getShipAtPosition(const int x, const int y);
	int getPositionOfShip(const int x, const int y);
	bool simAlreadyShot(const int x, const int y);
};

int digitCount(int number);
int digitAt(const int index, int number);
#endif
#include "Board.h"
#include <iostream>
#include <assert.h>
#include <string.h>
#include "Game.h"
#include "Ships.h"
#include "Constants.h"
#include "Command.h"
#include "Enums.h"
#include "Player.h"
using namespace std;

Board::Board() {
	boardHeight = BOARD_HEIGHT_DEFAULT;
	boardWidth = BOARD_WIDTH_DEFAULT;
	points = NULL;
	setSize();
	extendedLogic = false;
}

Board::Board(const Board& obj) {
	boardHeight = obj.boardHeight;
	boardWidth = obj.boardWidth;
	extendedLogic = obj.extendedLogic;
	points = NULL;
	setSize(obj);
}

Board::~Board() {
	deletePoints();
}

void Board::deletePoints() {
	if (points == NULL) {
		return;
	}
	for (int i = 0; i < boardHeight; i++) {
		for (int j = 0; j < boardWidth; j++) {
			delete points[i][j];
		}
		delete[] points[i];
	}
	delete[] points;
}

bool Board::placeShip(ShipCreatingCmd* cmd) {
	assert(cmd->x < boardWidth&& cmd->y < boardHeight);
	if (!isPositionValid(cmd)) {
		return false;
	}
	int xChange, yChange;
	getXAndYChangeFromDirection(cmd->getDirection(), &xChange, &yChange);
	setShipInPosition(cmd);
	return true;
}
const char* Board::isPositionValid(int x, int y, const char playerName, Directions direction, Ship* ship, bool isSimulated) {
	int xChange, yChange;
	getXAndYChangeFromDirection(direction, &xChange, &yChange);
	if (isSimulated && wontBeAbleToMakeAnotherMove(x, y, direction)) {
		return "WON'T BE ABLE TO MAKE ANOTHER MOVE";
	}
	for (int i = 0; i < ship->length; i++) {
		if (!isWithinBounds(x, y)) {
			return (playerName != NULL ? "NOT IN STARTING POSITION" : "SHIP WENT FROM BOARD");
		}
		if (playerName != NULL && !isOccupiedByPlayer(x, y, playerName)) {
			return "NOT IN STARTING POSITION";
		}
		if (points[y][x]->isReef) {
			return "PLACING SHIP ON REEF";
		}
		if (tooCloseToOtherShip(x, y, ship)) {
			return "PLACING SHIP TOO CLOSE TO OTHER SHIP";
		}
		x += xChange;
		y += yChange;
	}
	return NULL;
}

bool Board::isPositionValid(ShipCreatingCmd* cmd) {
	const char* errorMsg = isPositionValid(cmd->x, cmd->y, cmd->playerName, cmd->getDirection(), cmd->ship, cmd->isSimulated);
	if (errorMsg != NULL) {
		cmd->setErrorMsg(errorMsg);
		return false;
	}
	return true;
}

bool Board::isPositionValid(MoveCmd* cmd) {
	const char* errorMsg = isPositionValid(cmd->newX, cmd->newY, NULL, cmd->newDirection, cmd->ship, cmd->isSimulated);
	if (errorMsg != NULL) {
		cmd->setErrorMsg(errorMsg);
		return false;
	}
	return true;
}

bool Board::tooCloseToOtherShip(int x, int y, Ship* ship) {
	return (isOccupiedByOtherShip(x - 1, y, ship)
		|| isOccupiedByOtherShip(x + 1, y, ship)
		|| isOccupiedByOtherShip(x, y - 1, ship)
		|| isOccupiedByOtherShip(x, y + 1, ship)
		|| isOccupiedByOtherShip(x + 1, y + 1, ship)
		|| isOccupiedByOtherShip(x - 1, y + 1, ship)
		|| isOccupiedByOtherShip(x + 1, y - 1, ship)
		|| isOccupiedByOtherShip(x - 1, y - 1, ship));
}

bool Board::isOccupiedByOtherShip(int x, int y, Ship* ship) {
	return isWithinBounds(x, y) && points[y][x]->occupyingShip != NULL && points[y][x]->occupyingShip != ship;
}

char Board::playerOfPosition(const int x, const int y) {
	return (isWithinBounds(x, y) ? points[y][x]->occupyingPlayer : NULL);
}

int Board::getHeight() {
	return boardHeight;
}

int Board::getWidth() {
	return boardWidth;
}

void Board::print(PrintCmd* cmd) {
	const bool isExtended = cmd->isExtended();
	if (isExtended) printWidthIndices();
	for (int i = 0; i < boardHeight; i++) {
		if (isExtended) printHeightIndices(i);
		for (int j = 0; j < boardWidth; j++) {
			points[i][j]->print(cmd, isExtended, cmd->isSimulated);
		}
		printf("\n");
	}
}

void Board::printWidthIndices() {
	for (int i = 0; i < widthDigitCount; i++) {
		for (int j = 0; j < heightDigitCount; j++) {
			printf(" ");
		}
		for (int j = 0; j < boardWidth; j++) {
			printf("%d", (int)(j / pow(10, (widthDigitCount - i - 1))) % 10);
		}
		printf("\n");
	}
}

void Board::printHeightIndices(int row) {
	printf("%0*d", heightDigitCount, row);
}

int digitCount(int number) {
	int count = 0;
	while (number > 0) {
		number /= 10;
		count++;
	}
	return count;
}

bool Board::isWithinBounds(int x, int y) {
	return (x >= 0 && x < boardWidth && y >= 0 && y < boardHeight);
}

bool Board::shotHit(ShootCmd* cmd, const char shotPlayerName) {
	int x = cmd->x;
	int y = cmd->y;
	if (!isWithinBounds(x, y)) {
		cmd->setErrorMsg("FIELD DOES NOT EXIST");
		return false;
	}
	Point* point = points[y][x];
	if (point->occupyingShip == NULL) {
		return false;
	}
	if (point->occupyingShip->playerName != shotPlayerName) {
		assert(!"You tried to shoot yourself");
		return false;
	}
	return point->occupyingShip->shotHit(point->shipPosition);
}

void Point::print(PrintCmd* cmd, const bool extendedPrinting, const bool isSimulated) {
	if (cmd->isPlayer() && !cmd->player->canSee(x, y, isSimulated)) {
		printf("%c", SIGN_UNKNOWN);
		return;
	}
	if (isReef) {
		printf("%c", SIGN_REEF);
		return;
	}
	if (occupyingShip == NULL) {
		printf("%c", SIGN_NOTHING);
		return;
	}
	if (isSimulated && simulationDestroyed) {
		printf("%c", SIGN_SHIP_DESTROYED);
	}
	if (!extendedPrinting) {
		printf("%c", ((occupyingShip->getCharToPrint(shipPosition) == SIGN_SHIP_DESTROYED) ? SIGN_SHIP_DESTROYED : SIGN_SHIP_PRESENT_DEFAULT));
		return;
	}
	printf("%c", occupyingShip->getCharToPrint(shipPosition));
}

void Board::setSize(const int height, const int width) {
	if (points != NULL) {
		deletePoints();
	}
	boardHeight = height;
	boardWidth = width;
	heightDigitCount = digitCount(boardHeight - 1);
	widthDigitCount = digitCount(boardWidth - 1);
	points = new Point * *[boardHeight];
	for (int i = 0; i < boardHeight; i++) {
		points[i] = new Point * [boardWidth];
		for (int j = 0; j < boardWidth; j++) {
			Point* point = new Point();
			point->isReef = false;
			point->occupyingShip = NULL;
			point->shipPosition = UNINITIALIZED_INT;
			point->simulationAlreadyShot = false;
			if (i == boardHeight / 2) {
				point->occupyingPlayer = NULL;
			} else {
				point->occupyingPlayer = (i < boardHeight / 2) ? PLAYER_1 : PLAYER_2;
			}
			point->x = j;
			point->y = i;
			points[i][j] = point;
		}
	}
}

void Board::setSize(BoardSizeCmd* cmd) {
	setSize(cmd->height, cmd->width);
}

void Board::setReef(ReefCmd* cmd) {
	int x = cmd->x;
	int y = cmd->y;
	if (!isWithinBounds(x, y)) {
		cmd->setErrorMsg("REEF IS NOT PLACED ON BOARD");
		return;
	}
	points[y][x]->isReef = true;
}

void Board::initPositions(const InitPositionsCmd* cmd) {
	for (int i = 0; i < boardHeight; i++) {
		for (int j = 0; j < boardWidth; j++) {
			Point* point = points[i][j];
			if (i >= cmd->y1 && i <= cmd->y2 && j >= cmd->x1 && j <= cmd->x2) {
				point->setOccupyingPlayer(cmd->playerName);
			} else if (point->occupyingPlayer == cmd->playerName) {
				point->occupyingPlayer = NULL;
			}
		}
	}
}

bool Board::moveShip(MoveCmd* cmd) {
	Ship* ship = cmd->ship;
	if (ship == NULL) {
		assert(!"No ship found for MOVE COMMAND");
		return false;
	}
	if (!ship->canMove(cmd)) {
		return false;
	}
	setMoveParameters(cmd);
	if (!isPositionValid(cmd)) {
		return false;
	}
	unsetShipFromOldPosition(cmd);
	setShipInPosition(cmd);
	ship->moved(cmd);
	return true;
}

void Board::setShipInPosition(MoveCmd* cmd) {
	setShipInPosition(cmd->newX, cmd->newY, cmd->newDirection, cmd->ship, cmd->playerName, cmd->isSimulated);
}
void Board::setShipInPosition(ShipCreatingCmd* cmd) {
	setShipInPosition(cmd->x, cmd->y, cmd->getDirection(), cmd->ship, cmd->playerName, cmd->isSimulated);
}

void Board::setShipInPosition(int x, int y, Directions direction, Ship* ship, const char playerName, const bool isSimulated) {
	if (isSimulated) {
		ship->simulatedX = x;
		ship->simulatedY = y;
		ship->simulatedDirection = direction;
	} else {
		ship->x = x;
		ship->y = y;
		ship->direction = direction;
	}
	int xChange, yChange;
	getXAndYChangeFromDirection(direction, &xChange, &yChange);
	for (int i = 0; i < ship->length; i++) {
		points[y][x]->occupyingShip = ship;
		points[y][x]->shipPosition = i;
		points[y][x]->setOccupyingPlayer(playerName);
		x += xChange;
		y += yChange;
	}
}

void Board::unsetShipFromOldPosition(MoveCmd* cmd) {
	Ship* ship = cmd->ship;
	int oldX = ship->x;
	int oldY = ship->y;
	int xChange, yChange;
	Directions direction = ship->direction;
	if (cmd->isSimulated) {
		direction = ship->simulatedDirection;
		oldX = ship->simulatedX;
		oldY = ship->simulatedY;
	}
	getXAndYChangeFromDirection(direction, &xChange, &yChange);
	for (int i = 0; i < ship->length; i++) {
		points[oldY][oldX]->occupyingShip = NULL;
		points[oldY][oldX]->shipPosition = UNINITIALIZED_INT;
		oldX += xChange;
		oldY += yChange;
	}
}

void Board::setMoveParameters(MoveCmd* cmd) {
	Ship* ship = cmd->ship;
	int length = ship->length;
	int oldX = ship->x;
	int oldY = ship->y;
	Directions dir = ship->direction;
	if (cmd->isSimulated) {
		oldX = ship->simulatedX;
		oldY = ship->simulatedY;
		dir = ship->simulatedDirection;
	}
	MoveParams* params = new MoveParams(oldX, oldY, length, dir, cmd->getMoveDir());
	cmd->newDirection = params->direction;
	cmd->newX = params->x;
	cmd->newY = params->y;
	delete params;
}

MoveParams::MoveParams(const int x, const int y, const int length, const Directions dir, const MoveDir moveDir) {
	this->x = this->y = UNINITIALIZED_INT;
	this->direction = Directions::INVALID;
	int xChange = 0;
	int yChange = 0;
	getXAndYChangeFromDirection(dir, &xChange, &yChange);
	switch (moveDir) {
	case MoveDir::FORWARD:
		this->direction = dir;
		this->x = x - xChange;
		this->y = y - yChange;
		break;
	case MoveDir::RIGHT:
		this->direction = (Directions)(((int)dir + RIGHT_MOVE_DIRECTION_CHANGE) % NUMBER_OF_DIRECTIONS);
		if (xChange != 0) {
			this->x = x - xChange;
			this->y = y - (xChange * (length - 1));
		} else {
			this->y = y - yChange;
			this->x = x + (yChange * (length - 1));
		}
		break;
	case MoveDir::LEFT:
		this->direction = (Directions)(((int)dir + LEFT_MOVE_DIRECTION_CHANGE) % NUMBER_OF_DIRECTIONS);
		if (xChange != 0) {
			this->x = x - xChange;
			this->y = y + (xChange * (length - 1));
		} else {
			this->y = y - yChange;
			this->x = x - (yChange * (length - 1));
		}
		break;
	}
}

void Board::saveReefs() {
	for (int i = 0; i < boardHeight; i++) {
		for (int j = 0; j < boardWidth; j++) {
			if (points[i][j]->isReef) {
				printf("%s %d %d\n", OPERATION_REEF, i, j);
			}
		}
	}
}

void Board::saveInitPositions(const char playerName) {
	int maxX, maxY, minX, minY;
	maxX = maxY = -1;
	minY = minX = INT_MAX;
	for (int i = 0; i < boardHeight; i++) {
		for (int j = 0; j < boardWidth; j++) {
			if (isOccupiedByPlayer(j, i, playerName)) {
				if (j < minX) {
					minX = j;
				}
				if (j > maxX) {
					maxX = j;
				}
				if (i < minY) {
					minY = i;
				}
				if (i > maxY) {
					maxY = i;
				}
			}
		}
	}
	printf("%s %c %d %d %d %d\n", OPERATION_INIT_POSITION, playerName, minY, minX, maxY, maxX);
}

bool Board::isOccupiedByPlayer(const int x, const int y, const char playerName) {
	char playerOfPos = playerOfPosition(x, y);
	return playerOfPos == PLAYERS_BOTH || playerOfPos == playerName;
}

void Point::setOccupyingPlayer(const char name) {
	if (occupyingPlayer == NULL) {
		occupyingPlayer = name;
		return;
	}
	if (occupyingPlayer != name) {
		occupyingPlayer = PLAYERS_BOTH;
		return;
	}
	occupyingPlayer = name;
}

void Board::setSize(const Board& obj) {
	setSize(boardHeight, boardWidth);
	for (int i = 0; i < boardHeight; i++) {
		for (int j = 0; j < boardWidth; j++) {
			Point* point = points[i][j];
			Point* objPoint = obj.points[i][j];
			point->isReef = objPoint->isReef;
			point->occupyingPlayer = objPoint->occupyingPlayer;
			point->shipPosition = objPoint->shipPosition;
			Ship* ship = objPoint->occupyingShip;
			if (ship != NULL) {
				ship->simulatedX = ship->x;
				ship->simulatedY = ship->y;
				ship->simulatedDirection = ship->direction;
				point->occupyingShip = ship;
			}
		}
	}
}

void Board::simulateHit(const int x, const int y) {
	if (!isWithinBounds(x, y) || points[y][x] == NULL) {
		return;
	}
	points[y][x]->simulationAlreadyShot = true;
	points[y][x]->simulationDestroyed = true;
}

bool Board::isEnemyShip(const int x, const int y, const char playerName) {
	if (!isWithinBounds(x, y)) {
		return false;
	}
	return (points[y][x]->occupyingShip == NULL) ? false
		: points[y][x]->occupyingShip->playerName != playerName;
}

Ship* Board::getShipAtPosition(const int x, const int y) {
	return points[y][x]->occupyingShip;
}

int Board::getPositionOfShip(const int x, const int y) {
	return points[y][x]->shipPosition;
}

bool Board::simAlreadyShot(const int x, const int y) {
	if (!isWithinBounds(x, y)) {
		return false;
	}
	return points[y][x]->simulationAlreadyShot;
}

bool Board::wontBeAbleToMakeAnotherMove(const int x, const int y, const Directions direction) {
	return isFacingEdge(x, y, direction) || isFacingReef(x, y, direction);
}

bool Board::isFacingEdge(const int x, const int y, const Directions direction) {
	return ((x == 0 && direction == Directions::LEFT)
		|| (x == boardWidth - 1 && direction == Directions::RIGHT)
		|| (y == 0 && direction == Directions::UP)
		|| (y == boardHeight - 1 && direction == Directions::DOWN));
}

bool Board::isFacingReef(const int x, const int y, const Directions direction) {
	return ((isWithinBounds(x - 1, y) && points[y][x - 1]->isReef && direction == Directions::LEFT)
		|| (isWithinBounds(x + 1, y) && points[y][x + 1]->isReef && direction == Directions::RIGHT)
		|| (isWithinBounds(x, y - 1) && points[y - 1][x]->isReef && direction == Directions::UP)
		|| (isWithinBounds(x, y + 1) && points[y + 1][x]->isReef && direction == Directions::DOWN));
}
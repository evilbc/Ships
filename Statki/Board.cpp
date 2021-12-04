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
	if (!isPositionValid(cmd)) return false;
	int xChange, yChange;
	getXAndYChangeFromDirection(cmd->getDirection(), &xChange, &yChange);
	setShipInPosition(cmd);
	return true;
}
const char* Board::isPositionValid(int x, int y, const char playerName, Directions direction, Ship* ship) {
	int xChange, yChange;
	getXAndYChangeFromDirection(direction, &xChange, &yChange);
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
	const char* errorMsg = isPositionValid(cmd->x, cmd->y, cmd->playerName, cmd->getDirection(), cmd->ship);
	if (errorMsg != NULL) {
		cmd->setErrorMsg(errorMsg);
		return false;
	}
	return true;
}

bool Board::isPositionValid(MoveCmd* cmd) {
	const char* errorMsg = isPositionValid(cmd->newX, cmd->newY, NULL, cmd->newDirection, cmd->ship);
	if (errorMsg != NULL) {
		cmd->setErrorMsg(errorMsg);
		return false;
	}
	return true;
}

bool Board::tooCloseToOtherShip(int x, int y, Ship* ship) {
	if (isOccupiedByOtherShip(x - 1, y, ship)
		|| isOccupiedByOtherShip(x + 1, y, ship)
		|| isOccupiedByOtherShip(x, y - 1, ship)
		|| isOccupiedByOtherShip(x, y + 1, ship)
		|| isOccupiedByOtherShip(x + 1, y + 1, ship)
		|| isOccupiedByOtherShip(x - 1, y + 1, ship)
		|| isOccupiedByOtherShip(x + 1, y - 1, ship)
		|| isOccupiedByOtherShip(x - 1, y - 1, ship)) return true;
	return false;
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
			points[i][j]->print(cmd, isExtended);
		}
		cout << endl;
	}
}

void Board::printWidthIndices() {
	for (int i = 0; i < widthDigitCount; i++) {
		for (int j = 0; j < heightDigitCount; j++) {
			cout << ' ';
		}
		for (int j = 0; j < boardWidth; j++) {
			cout << (int)(j / pow(10, (widthDigitCount - i - 1))) % 10;
		}
		cout << endl;
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
	return (x >= 0 && x < boardWidth&& y >= 0 && y < boardHeight);
}

bool Board::shotHit(ShootCmd* cmd, const char shotPlayerName) {
	int x = cmd->x;
	int y = cmd->y;
	if (!isWithinBounds(x, y)) {
		cmd->setErrorMsg("FIELD DOES NOT EXIST");
		return false;
	}
	Point* point = points[y][x];
	if (point->occupyingShip != NULL) {
		return point->occupyingShip->shotHit(point->shipPosition);
	}
	return false;
}

void Point::print(PrintCmd* cmd, const bool extendedPrinting) {
	if (cmd->isPlayer() && !cmd->player->canSee(x, y)) {
		cout << SIGN_UNKNOWN;
		return;
	}
	if (isReef) {
		cout << SIGN_REEF;
		return;
	}
	if (occupyingShip == NULL) {
		cout << ' ';
		return;
	}
	if (!extendedPrinting) {
		cout << ((occupyingShip->getCharToPrint(shipPosition) == SIGN_SHIP_DESTROYED) ? SIGN_SHIP_DESTROYED : SIGN_SHIP_PRESENT_DEFAULT);
		return;
	}
	cout << occupyingShip->getCharToPrint(shipPosition);
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

void Board::initPositions(InitPositionsCmd* cmd) {
	for (int i = 0; i < boardHeight; i++) {
		for (int j = 0; j < boardWidth; j++) {
			Point* point = points[i][j];
			if (i >= cmd->y1 && i <= cmd->y2 && j >= cmd->x1 && j <= cmd->x2) {
				if (point->occupyingPlayer != NULL && point->occupyingPlayer != cmd->playerName) {
					point->occupyingPlayer = PLAYERS_BOTH;
				} else {
					point->occupyingPlayer = cmd->playerName;
				}
			} else if (point->occupyingPlayer == cmd->playerName) {
				point->occupyingPlayer = NULL;
			}
		}
	}
}

void Board::moveShip(MoveCmd* cmd) {
	Ship* ship = cmd->ship;
	if (ship == NULL) {
		assert(!"No ship found for MOVE COMMAND");
		return;
	}
	if (!ship->canMove(cmd)) {
		return;
	}
	setMoveParameters(cmd);
	if (!isPositionValid(cmd)) {
		return;
	}
	unsetShipFromOldPosition(cmd);
	setShipInPosition(cmd);
	ship->moved(cmd);
}

void Board::setShipInPosition(MoveCmd* cmd) {
	setShipInPosition(cmd->newX, cmd->newY, cmd->newDirection, cmd->ship, cmd->playerName);
}
void Board::setShipInPosition(ShipCreatingCmd* cmd) {
	setShipInPosition(cmd->x, cmd->y, cmd->getDirection(), cmd->ship, cmd->playerName);
}

void Board::setShipInPosition(int x, int y, Directions direction, Ship* ship, const char playerName) {
	ship->x = x;
	ship->y = y;
	ship->direction = direction;
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
	getXAndYChangeFromDirection(ship->direction, &xChange, &yChange);
	for (int i = 0; i < ship->length; i++) {
		points[oldY][oldX]->occupyingShip = NULL;
		points[oldY][oldX]->shipPosition = UNINITIALIZED_INT;
		oldX += xChange;
		oldY += yChange;
	}
}

void Board::setMoveParameters(MoveCmd* cmd) {
	int xChange = 0;
	int yChange = 0;
	Ship* ship = cmd->ship;
	int length = ship->length;
	int oldX = ship->x;
	int oldY = ship->y;
	getXAndYChangeFromDirection(ship->direction, &xChange, &yChange);
	switch (cmd->getMoveDir()) {
	case MoveDir::FORWARD:
		cmd->newDirection = ship->direction;
		cmd->newX = oldX - xChange;
		cmd->newY = oldY - yChange;
		return;
	case MoveDir::RIGHT:
		cmd->newDirection = (Directions)(((int)ship->direction + 1) % 4);
		if (xChange != 0) {
			cmd->newX = oldX - xChange;
			cmd->newY = oldY - (xChange * (length - 1));
		} else {
			cmd->newY = oldY - yChange;
			cmd->newX = oldX + (yChange * (length - 1));
		}
		return;
	case MoveDir::LEFT:
		cmd->newDirection = (Directions)(((int)ship->direction + 3) % 4);
		if (xChange != 0) {
			cmd->newX = oldX - xChange;
			cmd->newY = oldY + (xChange * (length - 1));
		} else {
			cmd->newY = oldY - yChange;
			cmd->newX = oldX - (yChange * (length - 1));
		}
		return;
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
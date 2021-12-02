#ifndef PLAYER
#define PLAYER
#include "Constants.h"

class Board;
enum class ShipTypes;
enum class Directions;
class ShipPointerArrayList;
class Ship;
class PlaceShipCmd;
class ShipCreatingCmd;
class SetFleetCmd;
class ShipCmd;
class MoveCmd;
class ShootCmd;
class SpyCmd;

class Player {
private:
	int maxCarrierNum, maxBattleshipNum, maxCruiserNum, maxDestroyerNum, piecesLeft;
	Board* board;
	char playerName;
	int getMaxNumberOfShipType(const ShipTypes shipType);
	ShipPointerArrayList* getListOfShipType(const ShipTypes shipType);
	Ship* createShip(const ShipTypes shipType);
	Ship* setShip(ShipCreatingCmd* cmd);
	void saveInitialPositions(ShipPointerArrayList* list);
	bool shipCanSee(int x, int y, ShipPointerArrayList* list);
public:
	Player(Board* board, char playerName);
	~Player();
	const char getPlayerName();
	bool isAi;
	ShipPointerArrayList* carriers, * battleships, * cruisers, * destroyers;
	void placeShip(PlaceShipCmd* cmd);
	void setFleet(const int maxCarriers = DEFAULT_NUMBER_OF_CARRIERS, const int maxBattleships = DEFAULT_NUMBER_OF_BATTLESHIPS, const int maxCruisers = DEFAULT_NUMBER_OF_CRUISERS, const int maxDestroyers = DEFAULT_NUMBER_OF_DESTROYERS);
	bool allShipsPlaced();
	void beShot(ShootCmd* cmd);
	int getPiecesLeft();
	void ship(ShipCmd* cmd);
	void moveShip(MoveCmd* cmd);
	void saveInitialPositions();
	void spy(SpyCmd* cmd);
	bool canSee(int x, int y);
	bool canShoot(ShootCmd* cmd);
};

#endif

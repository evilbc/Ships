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
class ShootingTargetPriorityQueue;
struct ShootingTarget;

struct AiMoveParams {
	Ship* ship;
	Board* simulatedBoard;
	int roundNum;
	bool madeAllMoves;
	int shotsLeft;
	ShootingTargetPriorityQueue* queue;
	int shipIndex;
};

struct AiShootingRange {
	int len;
	int minXRange;
	int maxXRange;
	int minYRange;
	int maxYRange;
	AiShootingRange(Ship* ship, Board* board);
};

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
	bool shipCanSee(int x, int y, ShipPointerArrayList* list, bool isSimulated = false);
	bool canShoot(ShootCmd* cmd, ShipPointerArrayList* list);
	ShipPointerArrayList* aiPlaceShips(ShipTypes type, Board* simulatedBoard);
	void cleanUpMockShips(ShipPointerArrayList* toDelete[NUMBER_OF_SHIP_TYPES]);
	void aiMove(ShipPointerArrayList* list, AiMoveParams* params);
	void aiMove(AiMoveParams* params);
	void tryToShoot(AiMoveParams* params);
	ShootingTarget getShootingTarget(const int x, const int y, Board* board, bool canSee = true);
	double calculateValueOfShot(Ship* ship, const int position);
	void findBestShots(AiMoveParams* params);
	void tryToSpy(AiMoveParams* params);
	void findRandomShots(AiMoveParams* params);
	void clearSimulatedPlanes();
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
	bool canSee(int x, int y, bool isSimulated = false);
	bool canShoot(ShootCmd* cmd);
	void handleAi(const int roundNum);
	bool aiAllShipsArePlaced;
	void aiPlaceShips();
};

#endif

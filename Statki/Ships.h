#ifndef SHIPS
#define SHIPS
#include "Constants.h"

class MoveCmd;
class ShootCmd;
class SaveInitPositions;
class PlaneLinkedList;
class SpyCmd;
enum class ShipTypes;
enum class Directions;
enum class MoveDir;
class Board;
struct ShipPosition {
	char sign;
	bool isDestroyed();
};

struct SpyPlane {
	int x, y;
	bool canSee(int x, int y);
};

struct AutoShootArgs {
	int x, y, movesLeft, shotsLeft, shootX, shootY, cannonX, cannonY, len;
	Directions direction;
	Board* board;
};

class Ship {
protected:
	void getCannonXAndY(bool isSimulated = false);
	void getCannonXAndY(AutoShootArgs* args);
	double distance(int x1, int y1, int x2, int y2);
	ShipPosition** positions;
	void initPositions();
	int roundOfLastMove, lastRoundMoveCount, roundOfLastShot, lastRoundShotCount, cannonX, cannonY;
	virtual bool isInShootingRange(int x, int y, bool isSimulated = false);
	bool isInRadarRange(int x, int y, bool isSimulated = false);
	void updateShotCount(const int cmdRoundNum);
	bool canAutoShoot(ShootCmd* cmd);
	bool canAutoShoot(AutoShootArgs args);
	bool shotFeasible(AutoShootArgs* args);
	void simulateMove(AutoShootArgs* args, MoveDir dir);
public:
	Ship(const int length, const ShipTypes type, const char playerName, const int maxMovesInRound = MAX_MOVES_DEFAULT);
	~Ship();
	const int maxMovesInRound;
	const int maxShotsInRound;
	const ShipTypes type;
	const int length;
	Directions direction, simulatedDirection;
	int x, y, simulatedX, simulatedY;
	char getCharToPrint(const int position);
	bool shotHit(const int position);
	bool canShoot(ShootCmd* cmd);
	bool canMove(MoveCmd* cmd);
	void moved(MoveCmd* cmd);
	void save(const int index);
	bool canSee(int x, int y, bool isSimulated = false);
	const char playerName;
	bool isEngineDestroyed();
	bool isCannonDestroyed();
	int shotsLeft(const int roundNum);
	int movesLeft(const int roundNum);
};

class Carrier : public Ship {
protected:
	bool isInShootingRange(int x, int y, bool isSimulated = false) override;
private:
	PlaneLinkedList* planes, * simulatedPlanes;
	void sendPlane(SpyCmd* cmd);
public:
	Carrier(const char playerName);
	~Carrier();
	void spy(SpyCmd* cmd);
	bool planesCanSee(int x, int y, bool isSimulated = false);
	bool canSpy(SpyCmd* cmd);
	void simSpy(const int x, const int y);
	void clearSimulatedPlanes();
};

class Battleship : public Ship {
public:
	Battleship(const char playerName);
};

class Cruiser : public Ship {
public:
	Cruiser(const char playerName);
};

class Destroyer : public Ship {
protected:
private:
public:
	Destroyer(const char playerName);
};
#endif
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
	bool canSee(int _x, int _y);
};

struct AutoShootArgs {
	int x, y, movesLeft, shotsLeft, shootX, shootY, cannonX, cannonY, len;
	Directions direction;
	Board* board;
};

class Ship {
protected:
	void getCannonXAndY();
	void getCannonXAndY(AutoShootArgs* args);
	double distance(int x1, int y1, int x2, int y2);
	ShipPosition** positions;
	void initPositions();
	int roundOfLastMove, lastRoundMoveCount, totalShotsMade, roundOfLastShot, lastRoundShotCount, cannonX, cannonY;
	const int maxMovesInRound;
	const int maxShotsInRound;
	const ShipTypes type;
	virtual bool isInShootingRange(int _x, int _y);
	bool isInRadarRange(int _x, int _y);
	void updateShotCount(const int cmdRoundNum);
	bool canAutoShoot(AutoShootArgs args);
	bool shotFeasible(AutoShootArgs* args);
	void simulateMove(AutoShootArgs* args, MoveDir dir);
public:
	Ship(const int _length, const ShipTypes _type, const char _playerName, const int _maxMovesInRound = MAX_MOVES_DEFAULT);
	~Ship();
	const int length;
	Directions direction, simulatedDirection;
	int x, y, simulatedX, simulatedY;
	char getCharToPrint(const int position);
	bool shotHit(const int position);
	bool canShoot(ShootCmd* cmd);
	bool canMove(MoveCmd* cmd);
	void moved(MoveCmd* cmd);
	void save(const char playerName, const int index);
	bool canSee(int _x, int _y);
	const char playerName;
};

class Carrier : public Ship {
protected:
	bool isInShootingRange(int x, int y) override;
private:
	PlaneLinkedList* planes;
	void sendPlane(SpyCmd* cmd);
public:
	Carrier(const char _playerName);
	~Carrier();
	void spy(SpyCmd* cmd);
	bool planesCanSee(int x, int y);
	bool canSpy(SpyCmd* cmd);
};

class Battleship : public Ship {
public:
	Battleship(const char _playerName);
};

class Cruiser : public Ship {
public:
	Cruiser(const char _playerName);
};

class Destroyer : public Ship {
protected:
private:
public:
	Destroyer(const char _playerName);
};
#endif
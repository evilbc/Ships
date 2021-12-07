#ifndef COMMANDS
#define COMMANDS
#define _CRT_SECURE_NO_WARNINGS

enum class ShipTypes;
enum class Directions;
enum class MoveDir;
class Board;
class Player;

class Ship;
class Command {
protected:
	char* fullOp;
	char* errorMsg;
public:
	Command(const char* args = "", const char* opName = "");
	~Command();
	char* getFullOp();
	void setErrorMsg(const char* msg);
	char* getErrorMsg();
	bool isError();
};

class XAndYCmd {
public:
	XAndYCmd();
	int y, x;
};

class ShipCreatingCmd : public Command, public XAndYCmd {
public:
	ShipCreatingCmd(char* args, const char* opName);
	ShipCreatingCmd(const int x, const int y, const Directions direction, const char playerName, const ShipTypes shipType, Ship* ship);
	~ShipCreatingCmd();
	int shipIndex;
	char direction, playerName, * shipType;
	bool isSimulated;
	ShipTypes getShipType();
	Directions getDirection();
	Ship* ship;
};

class PlaceShipCmd : public ShipCreatingCmd {
public:
	PlaceShipCmd(char* args);
};

class ShipCmd : public ShipCreatingCmd {
public:
	ShipCmd(char* args);
	~ShipCmd();
	char* shipFields;
};

class PrintCmd : public Command {
public:
	PrintCmd(char* args);
	PrintCmd(int type, Player* player);
	int type;
	Player* player;
	bool isExtended();
	bool isPlayer();
	bool isSimulated;
};

class SetFleetCmd : public Command {
public:
	SetFleetCmd(char* args);
	int numOfCarriers, numOfBattleships, numOfCruisers, numOfDestroyers;
	char playerName;
};

class NextPlayerCmd : public Command {
public:
	NextPlayerCmd(char* args);
	char playerName;
};

class BoardSizeCmd : public Command, public XAndYCmd {
public:
	BoardSizeCmd(char* args);
	int height, width;
};

class InitPositionsCmd : public Command {
public:
	InitPositionsCmd(char* args);
	char playerName;
	int y1, x1, y2, x2;
};

class ExtendedShips : public Command {
public:
	ExtendedShips(char* args);
};

class ReefCmd : public Command, public XAndYCmd {
public:
	ReefCmd(char* args);
};

class SetAiPlayerCmd : public Command {
public:
	SetAiPlayerCmd(char* args);
	char playerName;
};

class SrandCmd : public Command {
public:
	SrandCmd(char* args);
	int value;
};

class SaveCmd : public Command {
public:
	SaveCmd(char* args);
};

class AutoShootingCmd : public Command {
public:
	AutoShootingCmd(char* args);
};

class InformationCmd : public Command {
public:
	InformationCmd(char* args);
	char playerName;
};

class ShootCmd : public Command, public XAndYCmd {
public:
	ShootCmd(char* args);
	ShootCmd(const int x, const int y, const int roundNum, const ShipTypes shipType, const int index);
	~ShootCmd();
	int roundNum, shipIndex;
	char* shipType;
	ShipTypes getShipType();
	bool extendedLogic, isAuto, isSimulated;
	Board* board;
};

class MoveCmd : public Command {
public:
	MoveCmd(char* args);
	MoveCmd(const int roundNum, const MoveDir dir, const char playerName, Ship* ship, const bool isSimulated = true);
	~MoveCmd();
	int shipIndex, newX, newY, roundNum;
	char* shipType, dir;
	ShipTypes getShipType();
	Ship* ship;
	MoveDir getMoveDir();
	Directions newDirection;
	char playerName;
	bool isSimulated;
};

class SpyCmd : public Command, public XAndYCmd {
public:
	SpyCmd(char* args);
	int shipIndex, roundNum;
	bool isAuto;
};

#endif
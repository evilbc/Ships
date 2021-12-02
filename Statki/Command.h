#ifndef COMMANDS
#define COMMANDS
#define _CRT_SECURE_NO_WARNINGS

enum class ShipTypes;
enum class Directions;
enum class MoveDir;
class Player;

class Ship;
class Command {
protected:
	char* fullOp;
	char* errorMsg;
public:
	Command(char* args, const char* opName);
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
	~ShipCreatingCmd();
	int shipIndex;
	char direction, playerName, * shipType;
	ShipTypes getShipType();
	Directions getDirection();
	Ship* ship;
	//int getX();
	//int getY();
	//char getPlayerName();
	//Ship* getShip();
	//void setError(const char* msg) { setErrorMsg(msg); }
};

class PlaceShipCmd : public ShipCreatingCmd {
public:
	PlaceShipCmd(char* args);
};

class ShipCmd : public ShipCreatingCmd {
public:
	ShipCmd(char* args);
	~ShipCmd();
	char* shipFields, playerName;
};

class PrintCmd : public Command {
public:
	PrintCmd(char* args);
	int type;
	Player* player;
	bool isExtended();
	bool isPlayer();
};

class SetFleetCmd : public Command {
public:
	SetFleetCmd(char* args);
	int numOfCarriers, numOfBattleships, numOfCruisers, numOfDestroyers;
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
	~ShootCmd();
	int roundNum, shipIndex;
	char* shipType;
	ShipTypes getShipType();
	bool extendedLogic;
};

class MoveCmd : public Command {
public:
	MoveCmd(char* args);
	~MoveCmd();
	int shipIndex, newX, newY, roundNum;
	char* shipType, dir;
	ShipTypes getShipType();
	Ship* ship;
	MoveDir getMoveDir();
	Directions newDirection;
	//Directions getDirection();
	//int getX();
	//int getY();
	//char getPlayerName();
	//Ship* getShip();
	//void setError(const char* msg) { setErrorMsg(msg); }
};

class SpyCmd : public Command, public XAndYCmd {
public:
	SpyCmd(char* args);
	int shipIndex, roundNum;
};

#endif
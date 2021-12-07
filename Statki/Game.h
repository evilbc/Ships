#ifndef GAME
#define GAME
#define _CRT_SECURE_NO_WARNINGS
#include "Constants.h"

class Player;
class Board;
class Command;

class Game {
private:
	char* input;
	int roundNum;
	Player* player1;
	Player* player2;
	Player* currentPlayer;
	Board* board;
	Player* getPlayerFromChar(const char playerChar);
	void handleStateCommands();
	void handlePlayerCommands();
	bool isFinished;
	void setFleet();
	void nextPlayer();
	void placeShip();
	void shoot();
	void initPositions();
	bool extendedLogic;
	int srandValue;
	void ship();
	void move();
	void checkForError();
	Command* currentCmd;
	void print(const bool isPlayerPrint);
	void reef();
	void boardSize();
	void setAiPlayer();
	void save();
	void saveBoardSize();
	void saveNextPlayer();
	void saveInitialPositions();
	void saveExtendedShips();
	void saveAutoShooting();
	void saveAi();
	void saveSrand();
	void spy();
	bool isAutoShooting;
public:
	Game();
	~Game();
	void play();
};
#endif


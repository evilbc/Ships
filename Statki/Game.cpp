#include "Game.h"
#include <iostream>
#include <assert.h>
#include "Board.h"
#include "Player.h"
#include "Ships.h"
#include "Command.h"
using namespace std;


Game::Game() {
	board = new Board();
	player1 = new Player(board, PLAYER_1);
	player2 = new Player(board, PLAYER_2);
	isFinished = false;
	currentPlayer = NULL;
	input = new char[MAX_INPUT_LENGTH];
	extendedLogic = false;
	srandValue = UNINITIALIZED_INT;
	currentCmd = NULL;
	roundNum = 0;
	isAutoShooting = false;
}

void Game::play() {
	while (!isFinished && cin >> input) {
		if (strcmp(input, TOGGLE_STATE_COMMANDS) == 0) {
			handleStateCommands();
		} else {
			assert(strcmp(input, TOGGLE_PLAYER_1) == 0 || strcmp(input, TOGGLE_PLAYER_2) == 0);
			bool isPlayer1 = strcmp(input, TOGGLE_PLAYER_1) == 0;
			bool isPlayer2 = strcmp(input, TOGGLE_PLAYER_2) == 0;
			if ((isPlayer1 && currentPlayer == player1) || (isPlayer2 && currentPlayer == player2)) {
				printf(INVALID_OPEARTION_PRINTF, strcat(input, " "), "THE OTHER PLAYER EXPECTED");
				break;
			}
			currentPlayer = (isPlayer1) ? player1 : player2;
			handlePlayerCommands();
			roundNum++;
		}
	}

	delete[] input;
	delete player1;
	delete player2;
	delete board;
}

void Game::checkForError() {
	if (currentCmd == NULL) {
		return;
	}
	if (currentCmd->isError()) {
		printf(INVALID_OPEARTION_PRINTF, currentCmd->getFullOp(), currentCmd->getErrorMsg());
		isFinished = true;
	}
	delete currentCmd;
	currentCmd = NULL;
}

void Game::handleStateCommands() {
	cin >> input;
	while (strcmp(input, TOGGLE_STATE_COMMANDS) != 0 && !isFinished && cin) {
		if (strcmp(input, OPERATION_PRINT) == 0) {
			print(false);
		} else if (strcmp(input, OPERATION_SET_FLEET) == 0) {
			setFleet();
		} else if (strcmp(input, OPERATION_NEXT_PLAYER) == 0) {
			nextPlayer();
		} else if (strcmp(input, OPERATION_BOARD_SIZE) == 0) {
			boardSize();
		} else if (strcmp(input, OPERATION_INIT_POSITION) == 0) {
			initPositions();
		} else if (strcmp(input, OPERATION_REEF) == 0) {
			reef();
		} else if (strcmp(input, OPERATION_SHIP) == 0) {
			ship();
		} else if (strcmp(input, OPERATION_EXTENDED_SHIPS) == 0) {
			extendedLogic = true;
			board->extendedLogic = true;
		} else if (strcmp(input, OPERATION_SET_AI_PLAYER) == 0) {
			setAiPlayer();
		} else if (strcmp(input, OPERATION_SRAND) == 0) {
			cin >> srandValue;
			srand(srandValue);
		} else if (strcmp(input, OPERATION_SAVE) == 0) {
			save();
		} else if (strcmp(input, OPERATION_AUTO_SHOOTING) == 0) {
			isAutoShooting = true;
		} else {
			assert(!"No such state action");
		}
		checkForError();
		cin >> input;
	}
}

void Game::handlePlayerCommands() {
	if (currentPlayer->isAi) {
		currentPlayer->handleAi();
	}
	cin >> input;
	while (!(strcmp(input, TOGGLE_PLAYER_1) == 0 || strcmp(input, TOGGLE_PLAYER_2) == 0) && !isFinished && cin) {
		if (strcmp(input, OPERATION_PLACE_SHIP) == 0) {
			placeShip();
		} else if (strcmp(input, OPERATION_SHOOT) == 0) {
			shoot();
		} else if (strcmp(input, OPERATION_MOVE) == 0) {
			move();
		} else if (strcmp(input, OPERATION_SPY) == 0) {
			spy();
		} else if (strcmp(input, OPERATION_PRINT) == 0) {
			print(true);
		} else {
			assert(!"No such player action");
		}
		checkForError();
		cin >> input;
	}
}

void Game::setFleet() {
	char playerChar;
	int carrierNum, battleshipNum, cruiserNum, destroyerNum;
	int argCount = scanf(" %c %d %d %d %d", &playerChar, &carrierNum, &battleshipNum, &cruiserNum, &destroyerNum);
	assert(argCount == 1 || argCount == 5);
	Player* player = getPlayerFromChar(playerChar);
	if (argCount == 5) {
		player->setFleet(carrierNum, battleshipNum, cruiserNum, destroyerNum);
	} else if (argCount == 1) {
		player->setFleet();
	} else {
		assert(!"Incorrect number of arguments");
	}
}

void Game::nextPlayer() {
	char playerChar;
	cin >> playerChar;
	currentPlayer = (getPlayerFromChar(playerChar) == player1) ? player2 : player1;
}

Player* Game::getPlayerFromChar(char playerChar) {
	assert(playerChar == PLAYER_1 || playerChar == PLAYER_2);
	return (playerChar == PLAYER_1) ? player1 : player2;
}

void Game::placeShip() {
	cin.getline(input, MAX_INPUT_LENGTH);
	PlaceShipCmd* cmd = new PlaceShipCmd(input);
	currentCmd = cmd;
	int argCount = sscanf(input, "%d %d %c %d %s", &cmd->y, &cmd->x, &cmd->direction, &cmd->shipIndex, cmd->shipType);
	assert(argCount == 5);
	currentPlayer->placeShip(cmd);
}

void Game::shoot() {
	cin.getline(input, MAX_INPUT_LENGTH);
	ShootCmd* cmd = new ShootCmd(input);
	currentCmd = cmd;
	cmd->extendedLogic = extendedLogic;
	cmd->roundNum = roundNum;
	if (!extendedLogic) {
		int argCount = sscanf(input, "%d %d", &cmd->y, &cmd->x);
		assert(argCount == 2);
	} else {
		if (!isAutoShooting) {
			int argCount = sscanf(input, "%d %s %d %d", &cmd->shipIndex, cmd->shipType, &cmd->y, &cmd->x);
			assert(argCount == 4);
		} else {
			int argCount = sscanf(input, "%d %d", &cmd->y, &cmd->x);
			assert(argCount == 2);
			cmd->isAuto = true;
			cmd->board = board;
		}
		if (!currentPlayer->canShoot(cmd)) {
			return;
		}
	}
	if (!(player1->allShipsPlaced() && player2->allShipsPlaced())) {
		cmd->setErrorMsg("NOT ALL SHIPS PLACED");
		return;
	}
	Player* playerShot = (currentPlayer == player1) ? player2 : player1;
	playerShot->beShot(cmd);
	if (playerShot->getPiecesLeft() == 0) {
		isFinished = true;
		cout << currentPlayer->getPlayerName() << " won";
	}
}

void Game::initPositions() {
	cin.getline(input, MAX_INPUT_LENGTH);
	InitPositionsCmd* cmd = new InitPositionsCmd(input);
	currentCmd = cmd;
	int argCount = sscanf(input, " %c %d %d %d %d", &cmd->playerName, &cmd->y1, &cmd->x1, &cmd->y2, &cmd->x2);
	assert(argCount == 5);
	board->initPositions(cmd);
}

void Game::ship() {
	cin.getline(input, MAX_INPUT_LENGTH);
	ShipCmd* cmd = new ShipCmd(input);
	int argCount = sscanf(input, " %c %d %d %c %d %s %s", &cmd->playerName, &cmd->y, &cmd->x, &cmd->direction, &cmd->shipIndex, cmd->shipType, cmd->shipFields);
	currentCmd = cmd;
	assert(argCount == 7);
	Player* player = getPlayerFromChar(cmd->playerName);
	player->ship(cmd);
}

void Game::move() {
	cin.getline(input, MAX_INPUT_LENGTH);
	MoveCmd* cmd = new MoveCmd(input);
	currentCmd = cmd;
	int argCount = sscanf(input, "%d %s %c", &cmd->shipIndex, cmd->shipType, &cmd->dir);
	assert(argCount == 3);
	cmd->roundNum = roundNum;
	cmd->playerName = currentPlayer->getPlayerName();
	currentPlayer->moveShip(cmd);
}

void Game::print(const bool isPlayerPrint) {
	int type;
	cin >> type;
	PrintCmd* cmd = new PrintCmd(input);
	currentCmd = cmd;
	cmd->type = type;
	if (isPlayerPrint) {
		cmd->player = currentPlayer;
	}
	board->print(cmd);
	if (!isPlayerPrint) {
		cout << "PARTS REMAINING:: " << PLAYER_1 << " : " << player1->getPiecesLeft()
			<< " " << PLAYER_2 << " : " << player2->getPiecesLeft() << endl;
	}

}

void Game::reef() {
	cin.getline(input, MAX_INPUT_LENGTH);
	ReefCmd* cmd = new ReefCmd(input);
	currentCmd = cmd;
	int argCount = sscanf(input, "%d %d", &cmd->y, &cmd->x);
	assert(argCount == 2);
	board->setReef(cmd);
}

void Game::spy() {
	cin.getline(input, MAX_INPUT_LENGTH);
	SpyCmd* cmd = new SpyCmd(input);
	cmd->roundNum = roundNum;
	currentCmd = cmd;
	if (!isAutoShooting) {
		int argCount = sscanf(input, "%d %d %d", &cmd->shipIndex, &cmd->y, &cmd->x);
		assert(argCount == 3);
	} else {
		int argCount = sscanf(input, "%d %d", &cmd->y, &cmd->x);
		assert(argCount == 2);
		cmd->isAuto = true;
	}
	currentPlayer->spy(cmd);
}

void Game::boardSize() {
	cin.getline(input, MAX_INPUT_LENGTH);
	BoardSizeCmd* cmd = new BoardSizeCmd(input);
	currentCmd = cmd;
	int argCount = sscanf(input, "%d %d", &cmd->height, &cmd->width);
	assert(argCount == 2);
	board->setSize(cmd);
}

void Game::setAiPlayer() {
	char playerName;
	cin >> playerName;
	getPlayerFromChar(playerName)->isAi = true;
}

void Game::save() {
	cout << TOGGLE_STATE_COMMANDS << endl;
	saveBoardSize();
	saveNextPlayer();
	saveInitialPositions();
	board->saveReefs();
	saveExtendedShips();
	saveAutoShooting();
	saveAi();
	saveSrand();
	cout << TOGGLE_STATE_COMMANDS << endl;
}

void Game::saveBoardSize() {
	printf("%s %d %d\n", OPERATION_BOARD_SIZE, board->getHeight(), board->getWidth());
}

void Game::saveNextPlayer() {
	Player* nextPlayer = (currentPlayer == player1) ? player2 : player1;
	printf("%s %c\n", OPERATION_NEXT_PLAYER, nextPlayer->getPlayerName());
}

void Game::saveInitialPositions() {
	player1->saveInitialPositions();
	player2->saveInitialPositions();
}
void Game::saveExtendedShips() {
	if (extendedLogic) {
		cout << OPERATION_EXTENDED_SHIPS << endl;
	}
}
void Game::saveAutoShooting() {
	if (isAutoShooting) {
		cout << OPERATION_AUTO_SHOOTING << endl;
	}
}
void Game::saveAi() {
	if (player1->isAi) {
		printf("%s %c\n", OPERATION_SET_AI_PLAYER, player1->getPlayerName());
	}
	if (player2->isAi) {
		printf("%s %c\n", OPERATION_SET_AI_PLAYER, player2->getPlayerName());
	}
}

void Game::saveSrand() {
	if (srandValue != UNINITIALIZED_INT)
		printf("%s %d\n", OPERATION_SRAND, srandValue + 1);
}

#ifndef CONSTANTS
#define CONSTANTS

#define BOARD_HEIGHT_DEFAULT 21
#define BOARD_WIDTH_DEFAULT 10

#define SIGN_SHIP_PRESENT_DEFAULT '+'
#define SIGN_SHIP_PRESENT_ENGINE '%'
#define SIGN_SHIP_PRESENT_CANNON '!'
#define SIGN_SHIP_PRESENT_RADAR '@'
#define SIGN_SHIP_DESTROYED 'x'
#define SIGN_UNKNOWN '?'
#define SIGN_NOTHING ' '

#define SIGN_REEF '#'

#define MAX_INPUT_LENGTH 100
#define MAX_INPUT_SHIP_TYPE_LENGTH 4
#define MAX_INPUT_SHIP_SIZE_LENGTH 6

#define PLAYER_1 'A'
#define PLAYER_2 'B'
#define PLAYERS_BOTH 'C'

#define DEFAULT_NUMBER_OF_CARRIERS 1
#define DEFAULT_NUMBER_OF_BATTLESHIPS 2
#define DEFAULT_NUMBER_OF_CRUISERS 3
#define DEFAULT_NUMBER_OF_DESTROYERS 4

#define TOGGLE_STATE_COMMANDS "[state]"
#define TOGGLE_PLAYER_1 "[playerA]"
#define TOGGLE_PLAYER_2 "[playerB]"

#define CARRIER_ABBR "CAR"
#define BATTLESHIP_ABBR "BAT"
#define CRUISER_ABBR "CRU"
#define DESTROYER_ABBR "DES"

#define INVALID_OPEARTION_PRINTF "INVALID OPERATION \"%s\": %s\n"

#define OPERATION_PRINT "PRINT"
#define OPERATION_SET_FLEET "SET_FLEET"
#define OPERATION_NEXT_PLAYER "NEXT_PLAYER"
#define OPERATION_BOARD_SIZE "BOARD_SIZE"
#define OPERATION_INIT_POSITION "INIT_POSITION"
#define OPERATION_REEF "REEF"
#define OPERATION_SHIP "SHIP"
#define OPERATION_EXTENDED_SHIPS "EXTENDED_SHIPS"
#define OPERATION_SET_AI_PLAYER "SET_AI_PLAYER"
#define OPERATION_SRAND "SRAND"
#define OPERATION_SAVE "SAVE"
#define OPERATION_AUTO_SHOOTING "AUTO_SHOOTING"
#define OPERATION_INFORMATION "INFORMATION"
#define OPERATION_PLACE_SHIP "PLACE_SHIP"
#define OPERATION_SHOOT "SHOOT"
#define OPERATION_MOVE "MOVE"
#define OPERATION_SPY "SPY"


#define BASIC_PRINT 0
#define EXTENDED_PRINT 1
#define OP_SHIP_DESTROYED_FIELD '0'
#define OP_SHIP_PRESENT_FIELD '1'

#define UNINITIALIZED_INT -1

#define SHIP_RADAR_POSITION 0
#define SHIP_CANNON_POSITION 1
#define SHIP_ENGINE_POSITION length - 1

#define MAX_MOVES_DEFAULT 3
#define MAX_MOVES_CARRIER 2

#define LENGTH_CARRIER 5
#define LENGTH_BATTLESHIP 4
#define LENGTH_CRUISER 3
#define LENGTH_DESTROYER 2

#define DIRECTION_UP 'N'
#define DIRECTION_RIGHT 'E'
#define DIRECTION_DOWN 'S'
#define DIRECTION_LEFT 'W'

#define NUMBER_OF_SHIP_TYPES 4
#define AI_MAX_TRIES 10
#define LENGTH_MULTIPLIER 0.05
#define CANNON_MULTIPLIER 4.5
#define ENGINE_MULTIPLIER 2
#define RADAR_MULTIPLIER 1.5
#define BASE_MULTIPLIER 0.2
#define SPYING_VALUE 1
#define BLIND_SHOT_VALUE 0

#define LEFT_MOVE_DIRECTION_CHANGE 3
#define RIGHT_MOVE_DIRECTION_CHANGE 1
#define NUMBER_OF_DIRECTIONS 4

//#define NDEBUG
#endif
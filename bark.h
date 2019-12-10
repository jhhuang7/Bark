// Hash includes for the program.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

// Hash defines to avoid magic numbers.
#define HUMAN 'h'
#define AUTO 'a'
#define INITHAND 5
#define HANDSIZE 6
#define MINDIMENSION 3
#define MAXDIMENSION 100
#define EMPTYBOARDSPACE '.'
#define EMPTYFILEBDSPACE '*'
#define BUFFERSIZE 200
#define MAXDECKSIZE 234
#define MINDECKSIZE 11
#define NUMPLAYERS 2
#define PLAYERONE 0
#define PLAYERTWO 1

// Enum to represent exit status.
typedef enum {
    NORMAL = 0,
    INCNUMARGS = 1,
    INCARGTYPE = 2,
    BADDECK = 3,
    BADSAVE = 4,
    SHORTDECK = 5,
    BOARDFULL = 6,
    ENDINPUT = 7
} Status;

// Card struct to hold rank and suit info.
typedef struct {
    char rank;
    char suit;
} Card;

// Deck struct to hold a list of cards.
typedef struct {
    char* deckfile;
    Card* cards;
    int numCards;
    int cardsPlayed;
} Deck;

// Player struct to hold important info about a player.
typedef struct {
    char type;
    Card hand[HANDSIZE];
    int score;
    int prevCardPos;
} Player;

// Game struct to hold all important info about the game.
typedef struct {
    Deck deck;
    Player players[NUMPLAYERS];
    int width;
    int height;
    Card** board;
    int playerTurn;
    int spacesFilled;
} Game;

// Function declarations.
Status show_message(Status status);
Status check_arguments(int argc, char** argv, Game* game);
bool valid_dimensions(int width, int height);
bool valid_card(char rank, char suit);
Status parse_deckfile(const char* deckfile, Game* game);
Status parse_savefile(const char* savefile, Game* game);
bool get_hand(const char* line, Game* game, int player);
bool get_board(Game* game, int width, int height, FILE* file);
void print_board(Game* game);
void print_hand(Game* game, int player);
void fill_hand(Game* game, int player);
void draw_card(Game* game, int player, int position);
Status get_move(Game* game, int player);
bool valid_move(Game* game, int card, int col, int row);
void save_game(Game* game, char* filename);
Status human_move(Game* game, int player);
void auto_move(Game* game, int player);
Status game_loop(Game* game);
bool is_greater(Game* game, char rank, int col, int row);
int score_pos(Game* game, int col, int row, char suit);
void cal_scores(Game* game);

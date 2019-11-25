// Hash includes for the program.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

// Hash defines to avoid magic numbers.
#define HUMAN 'h'
#define AUTO 'a'
#define HANDSIZE 6
#define MINDIMENSION 3
#define MAXDIMENSION 100
#define EMPTYBOARDSPACE '.'
#define EMPTYFILEBDSPACE '*'

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
    const char* deckfile;
    Card* cards;
    int numCards;
    int cardsPlayed;
} Deck;

// Player struct to hold important info about a player.
typedef struct {
    char type;
    Card hand[HANDSIZE];
    int score;
} Player;

// Game struct to hold all important info about the game.
typedef struct {
    Deck deck;
    Player p1;
    Player p2;
    int width;
    int height;
    Card** board;
} Game;

// Function declarations.
Status show_message(Status status);
Status check_arguments(int argc, char** argv, Game* game);
Status parse_deckfile(const char* deckfile, Game* game);
Status parse_savefile(const char* savefile, Game* game);
void print_board(Game* game);
Status game_loop(Game* game);

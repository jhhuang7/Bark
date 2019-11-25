#include "bark.h"

/**
 * Puts an error message to standard error.
 * Params: a status enum.
 * Returns: the given status num.
 */
Status show_message(Status status) {
    const char* messages[] = {"",
            "Usage: bark savefile p1type p2type\n"
            "bark deck width height p1type p2type\n",
            "Incorrect arg types\n",
            "Unable to parse deckfile\n",
            "Unable to parse savefile\n",
            "Short deck\n",
            "Board full\n",
            "End of input\n"};
    fputs(messages[status], stderr);
    return status;
}

/**
 * Checks the given command line arguements and inits game info.
 * Params: argc and argv from main function.
 * Returns: an error status.
 */
Status check_arguments(int argc, char** argv, Game* game) {
    switch (argc) {
        case 4:
            if ((argv[2][0] != HUMAN && argv[2][0] != AUTO) || 
                    (argv[3][0] != HUMAN && argv[3][0] != AUTO)) {
                return INCARGTYPE;
            }
            game->p1.type = argv[2][0];
            game->p1.score = 0;
            game->p2.type = argv[3][0];
            game->p2.score = 0;
            
            return parse_savefile(argv[1], game);
        case 6:
            if ((argv[4][0] != HUMAN && argv[4][0] != AUTO) ||
                    (argv[5][0] != HUMAN && argv[5][0] != AUTO)) {
                return INCARGTYPE;
            }
            game->p1.type = argv[4][0];
            game->p1.score = 0;
            game->p2.type = argv[5][0];
            game->p2.score = 0;

            char* widtherr;
            int width = strtol(argv[2], &widtherr, 10);
            char* heighterr;
            int height = strtol(argv[3], &heighterr, 10);
            if (width < MINDIMENSION || width > MAXDIMENSION ||
                    strcmp(widtherr, "") != 0 || height < MINDIMENSION ||
                    height > MAXDIMENSION || strcmp(heighterr, "") != 0) {
                return INCARGTYPE;
            }
            game->width = width;
            game->height = height;
            game->board = malloc(sizeof(Card*) * height);
            for (int i = 0; i < height; i++) {
                game->board[i] = malloc(sizeof(Card) * width);
                for (int j = 0; j < width; j++) {
                    game->board[i][j].rank = EMPTYBOARDSPACE;
                    game->board[i][j].suit = EMPTYBOARDSPACE;
                }
            }

            return parse_deckfile(argv[1], game);
        default:
            return INCNUMARGS;
    }

    return NORMAL;
}

/**
 * Attempts to parse a deckfile given a filename and add it to the game.
 * Param: string of deckfile name, Game struct.
 * Returns: error status.
 */
Status parse_deckfile(const char* deckfile, Game* game) {
    game->deck.deckfile = deckfile;
    game->deck.cardsPlayed = 0;

    printf("%s\n", deckfile); // debug

    // Parse deckfile and check for errors.
    // -"Unable to parse deckfile\n"
    // -"Short deck\n"

    return NORMAL;
}

/**
 * Attempts to load a savefile given a filename and add it to the game.
 * Param: string of savefile name, Game struct.
 * Returns: error status.
 */
Status parse_savefile(const char* savefile, Game* game) {

    printf("%s\n", savefile); // debug

    // Parse savefile and check for errors.
    // -"Unable to parse savefile\n"
    // -"Unable to parse deckfile\n"
    // -"Short deck\n"
    // -"Board full\n"

    return NORMAL;
}

/**
 * Prints the board of the game.
 * Params: Game struct.
 * Returns: nothing (void).
 */
void print_board(Game* game) {
    for (int i = 0; i < game->height; i++) {
        for (int j = 0; j < game->width; j++) {
            printf("%c%c", game->board[i][j].rank, game->board[i][j].suit);
            if (j == game->width - 1) {
                printf("\n");
            }
        }
    }
}

/**
 * Runs the main logic of the game.
 * Params: Game struct.
 * Returns: a status depending what happens.
 */
Status game_loop(Game* game) {

    // Game loigc.

    return NORMAL;
}

/**
 * The main function of the program.
 */
int main(int argc, char** argv) {
    Game game;

    Status args = check_arguments(argc, argv, &game);
    if (args != NORMAL) {
        return show_message(args);
    }

    print_board(&game); // debug

    return game_loop(&game);
}

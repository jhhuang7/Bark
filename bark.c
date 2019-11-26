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
            if ((*(argv[2]) != HUMAN && *(argv[2]) != AUTO) || 
                    (*(argv[3]) != HUMAN && *(argv[3]) != AUTO)) {
                return INCARGTYPE;
            }
            game->players[PLAYERONE].type = *(argv[2]);
            game->players[PLAYERONE].score = 0;
            game->players[PLAYERONE].prevCardPos = INITHAND;
            game->players[PLAYERTWO].type = *(argv[3]);
            game->players[PLAYERTWO].score = 0;
            game->players[PLAYERTWO].prevCardPos = INITHAND;
            
            return parse_savefile(argv[1], game);
        case 6:
            if ((*(argv[4]) != HUMAN && *(argv[4]) != AUTO) ||
                    (*(argv[5]) != HUMAN && *(argv[5]) != AUTO)) {
                return INCARGTYPE;
            }
            game->players[PLAYERONE].type = *(argv[4]);
            game->players[PLAYERONE].score = 0;
            game->players[PLAYERONE].prevCardPos = INITHAND;
            game->players[PLAYERTWO].type = *(argv[5]);
            game->players[PLAYERTWO].score = 0;
            game->players[PLAYERTWO].prevCardPos = INITHAND;

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

    FILE* file = fopen(deckfile, "r");
    if (!file) {
        return BADDECK;
    }

    char line[BUFFERSIZE];
    if (!fgets(line, BUFFERSIZE, file)) {
        return BADDECK;
    }

    char* numerr;
    int numCards = strtol(line, &numerr, 10);
    if (*numerr != '\n' || numCards > MAXDECKSIZE) {
        return BADDECK;
    } else if (numCards < 11) {
        return SHORTDECK;
    }

    game->deck.cards = malloc(sizeof(Card) * numCards);
    game->deck.numCards = numCards;

    for (int i = 0; i < numCards; i++) {
        if (!fgets(line, BUFFERSIZE, file)) {
            free(game->deck.cards);
            return BADDECK;
        }
        if (((line[2] != '\n') && (line[2] != '\0')) ||
                line[0] < '1' || line[0] > '9' ||
                line[1] < 'A' || line[1] > 'Z') {
            free(game->deck.cards);
            return BADDECK;
        }

        game->deck.cards[i].rank = line[0];
        game->deck.cards[i].suit = line[1];
    }
    
    fclose(file);

    return NORMAL;
}

/**
 * Attempts to load a savefile given a filename and add it to the game.
 * Param: string of savefile name, Game struct.
 * Returns: error status.
 */
Status parse_savefile(const char* savefile, Game* game) {

    printf("%s\n", savefile); // debug

    FILE* file = fopen(savefile, "r");
    if (!file) {
        return BADSAVE;
    }

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
 * Prints the hand of a player.
 * Params: Game struct, player number int.
 * Returns: nothing (void).
 */
void print_hand(Game* game, int player) {
    if (player != PLAYERONE && player != PLAYERTWO) {
        return;
    }

    printf("Hand(%d): ", player + 1);
    for (int i = 0; i < HANDSIZE; i++) {
        printf("%c%c", game->players[player].hand[i].rank, 
            game->players[player].hand[i].suit);
        if (i != HANDSIZE - 1) {
            printf(" ");
        }
    }
    printf("\n");
}

/**
 * Fills the hand of a player.
 * Params: Game struct, player number int.
 * Returns: nothing (void).
 */
void fill_hand(Game* game, int player) {
    for (int i = 0; i < INITHAND; i++) {
        game->players[player].hand[i].rank = 
            game->deck.cards[game->deck.cardsPlayed].rank;
        game->players[player].hand[i].suit = 
            game->deck.cards[game->deck.cardsPlayed].suit;
        game->deck.cardsPlayed += 1;
    }
}

/**
 * Redistributes a player's hand after they draw a new card.
 * Params: Game struct, player number int, position of hand played.
 * Returns: nothing (void).
 */
void draw_card(Game* game, int player, int position) {
    for (int i = position; i < HANDSIZE; i++) {
        game->players[player].hand[i].rank = 
            game->players[player].hand[i + 1].rank;
        game->players[player].hand[i].suit = 
            game->deck.cards[i + 1].suit;
    }

    // Fill the last card with new card from deck.
    game->players[player].hand[INITHAND].rank = 
        game->deck.cards[game->deck.cardsPlayed].rank;
    game->players[player].hand[INITHAND].suit = 
        game->deck.cards[game->deck.cardsPlayed].suit;
    game->deck.cardsPlayed += 1;
}

/**
 * Asks the player for a move and if valid, place card on the board.
 * Params: Game struct.
 * Returns: nothing (void).
 */
void get_move(Game* game) {
    while(1) {
        printf("Move? ");

        // Check for if player is trying to SAVE

        /*
        if (valid_move) {
            // Store input info
            // Place card on board
            break;
        }
        */
        
    }
}

/**
 * Handles logic for a human move.
 * Params: Game struct, player number int.
 * Returns: nothing (void).
 */
void human_move(Game* game, int player) {

    print_board(game);

    draw_card(game, player, game->players[player].prevCardPos);

    print_hand(game, player);

    get_move(game);

}

/**
 * Runs the main logic of the game.
 * Params: Game struct.
 * Returns: a status depending what happens.
 */
Status game_loop(Game* game) {

    fill_hand(game, PLAYERONE);
    fill_hand(game, PLAYERTWO);

    // human_move(game, PLAYERONE);

    free(game->board);
    free(game->deck.cards);

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

    return game_loop(&game);
}

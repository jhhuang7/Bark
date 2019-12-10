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
            game->playerTurn = PLAYERONE;

            char* widtherr;
            int width = strtol(argv[2], &widtherr, 10);
            char* heighterr;
            int height = strtol(argv[3], &heighterr, 10);
            if (!valid_dimensions(width, height) || 
                    strcmp(widtherr, "") != 0 || strcmp(heighterr, "") != 0) {
                return INCARGTYPE;
            }
            game->width = width;
            game->height = height;
            game->spacesFilled = 0;
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
 * Checks if given board dimensions are valid.
 * Params: int width and height.
 * Returns true or false.
 */
bool valid_dimensions(int width, int height) {
    if (width < MINDIMENSION || width > MAXDIMENSION ||
            height < MINDIMENSION || height > MAXDIMENSION) {
        return false;
    }

    return true;
}

/**
 * Checks if a given card is valid.
 * Params: characters of rank, suit and end char.
 * Returns: true or false.
 */
bool valid_card(char rank, char suit) {
     if (rank < '1' || rank > '9' || suit < 'A' || suit > 'Z') {
        return false;
    }

    return true;
}

/**
 * Attempts to parse a deckfile given a filename and add it to the game.
 * Param: string of deckfile name, Game struct.
 * Returns: error status.
 */
Status parse_deckfile(const char* deckfile, Game* game) {
    game->deck.deckfile = malloc(sizeof(const char) * BUFFERSIZE);
    strcpy(game->deck.deckfile, deckfile);
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
    } else if (numCards < MINDECKSIZE) {
        return SHORTDECK;
    }

    game->deck.cards = malloc(sizeof(Card) * numCards);
    game->deck.numCards = numCards;

    for (int i = 0; i < numCards; i++) {
        if (!fgets(line, BUFFERSIZE, file)) {
            free(game->deck.cards);
            return BADDECK;
        }
        if (!valid_card(line[0], line[1]) || 
                ((line[2] != '\n') && (line[2] != '\0'))) {
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
 * Params: string of savefile name, Game struct.
 * Returns: error status.
 */
Status parse_savefile(const char* savefile, Game* game) {
    FILE* file = fopen(savefile, "r");
    if (!file) {
        return BADSAVE;
    }

    char line[BUFFERSIZE];
    if (!fgets(line, BUFFERSIZE, file)) {
        return BADSAVE;
    }

    int width, height, cardsPlayed, playerTurn;
    if ((sscanf(line, "%d %d %d %d", &width, &height, &cardsPlayed, 
            &playerTurn) != 4) || (playerTurn < 1 || playerTurn > 2 || 
            !valid_dimensions(width, height) || 
            !fgets(line, BUFFERSIZE, file))) {
        return BADSAVE;
    }

    line[strlen(line) - 1] = '\0';
    if (parse_deckfile(line, game) != NORMAL) {
        return BADDECK;
    }

    game->width = width;
    game->height = height;
    game->playerTurn = playerTurn - 1;
    game->deck.cardsPlayed = cardsPlayed - 1;  

    for (int i = 0; i < NUMPLAYERS; i++) {
        if (!fgets(line, BUFFERSIZE, file) || !get_hand(line, game, i)) {
            return BADSAVE;
        }
    }

    if (!get_board(game, width, height, file)) {
        return BADSAVE;
    }

    if (game->spacesFilled >= game->height * game->width) {
        fclose(file);
        return BOARDFULL;
    }

    fclose(file);

    return NORMAL;
}

/**
 * Fills a player's hand from a savefile.
 * Params: string line to parse, game struct, player number.
 * Returns: true or false based on success or not.
 */
bool get_hand(const char* line, Game* game, int player) {
    for (int i = 0; i < HANDSIZE; i++) {
        if ((line[2 * i] == '\0') || (line[2 * i] == '\n')) {
            return true;
        } else if (valid_card(line[2 * i], line[2 * i + 1])) {
            game->players[player].hand[i].rank = line[2 * i];
            game->players[player].hand[i].suit = line[2 * i + 1];
        } else {
            return false;
        }
    }

    return true;
}

/**
 * Fills a game board from a savefile.
 * Params: game struct, width and height numbers, the file pointer to be read.
 * Returns: rue or false based on success or not.
 */
bool get_board(Game* game, int width, int height, FILE* file) {
    game->board = malloc(sizeof(Card*) * height);
    game->spacesFilled = 0;

    for (int i = 0; i < height; i++) {
        game->board[i] = malloc(sizeof(Card) * width);
        for (int j = 0; j < width; j++) {
            game->board[i][j].rank = EMPTYBOARDSPACE;
            game->board[i][j].suit = EMPTYBOARDSPACE;
        }
    }

    for (int x = 0; x < height; x++) {
        for (int y = 0; y < width; y++) {
            int char1 = fgetc(file);
            int char2 = fgetc(file);
            if (char1 == EOF || char2 == EOF) {
                return false;
            }
            if ((char1 == EMPTYFILEBDSPACE) && (char2 == EMPTYFILEBDSPACE)) {
                continue;
            } else if (valid_card(char1, char2)) {
                game->board[x][y].rank = char1;
                game->board[x][y].suit = char2;
                game->spacesFilled += 1;
            } else {
                return false;
            }
        }

        if (x < height - 1) {
            char charend = fgetc(file);
            if (charend != '\n') {
                return false;
            }
        }
    }

    return true;
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
Status get_move(Game* game, int player) {
    while(1) {
        printf("Move? ");

        char move[BUFFERSIZE];
        int c, i = 0;
        do {
            c = fgetc(stdin);
            if (feof(stdin)) { 
                return ENDINPUT;
            }
            if (c == '\n') {
                break;
            }
            move[i] = (char)c;
            i++;
        } while(1);

        // Check for if player is trying to SAVE
        if (strncmp(move, "SAVE", 4) == 0) {
            char filename[BUFFERSIZE];
            memset(filename, '\0', sizeof(filename));
            strncpy(filename, move + 4, strlen(move) - 4);

            save_game(game, filename);
            continue;
        }

        int card, col, row;
        char lastit[BUFFERSIZE];
        int numvars = sscanf(move, "%d %d %d%s", &card, &col, &row, lastit);
        if (numvars != 3) {
            continue;
        }
        if (valid_move(game, card, col, row)) {
            // Place card on board
            game->board[row - 1][col - 1].rank 
                = game->players[player].hand[card - 1].rank;
            game->board[row - 1][col - 1].suit
                = game->players[player].hand[card - 1].suit;
            // Store input info
            game->spacesFilled += 1;
            game->players[player].prevCardPos = card - 1;
            break;
        }
    }

    return NORMAL;
}

/**
 * Checks if the given human input is a valid move.
 * Params: Game struct, ints of card pos played, col and row of board to place.
 * Returns: true of false if the move is valid.
 */
bool valid_move(Game* game, int card, int col, int row) {
    if (card < 1 || card > 6) {
        return false;
    }

    if (col < 1 || col > game->width) {
        return false;
    }

    if (row < 1 || row > game->height) {
        return false;
    }

    // Check if valid on board
    if (game->spacesFilled == 0) {
        return true;
    }

    int x = col - 1;
    int y = row - 1;
    int t = y - 1;
    int l = x - 1;
    int d = y + 1;
    int r = x + 1;

    if (t == -1) {
        t = game->height - 1;
    }
    if (l == -1) {
        l = game->width - 1;
    }
    if (d == game->height) {
        d = 0;
    }
    if (r == game->width) {
        r = 0;
    }

    if (game->board[y][x].rank == EMPTYBOARDSPACE 
            && game->board[y][x].suit == EMPTYBOARDSPACE) {
        if ((game->board[t][x].rank != EMPTYBOARDSPACE 
                && game->board[t][x].suit != EMPTYBOARDSPACE) ||
                (game->board[y][l].rank != EMPTYBOARDSPACE 
                && game->board[y][l].suit != EMPTYBOARDSPACE) ||
                (game->board[d][x].rank != EMPTYBOARDSPACE 
                && game->board[d][x].suit != EMPTYBOARDSPACE) ||
                (game->board[y][r].rank != EMPTYBOARDSPACE 
                && game->board[y][r].suit != EMPTYBOARDSPACE)) {
            return true;
        }
    }
    
    return false;
}

/**
 * Saves a game if player has entered a command to.
 * Params: Game struct, string of file to save to.
 * Returns: nothing (void).
 */
void save_game(Game* game, char* filename) {
    FILE* file;
    file = fopen(filename, "w");

    if (strlen(filename) < 1 || !file) {
        printf("Unable to save\n");
        return;
    }

    fprintf(file, "%d %d %d %d\n", game->width, game->height, 
        game->deck.cardsPlayed, game->playerTurn + 1);

    fprintf(file, "%s\n", game->deck.deckfile);

    for (int i = 0; i < HANDSIZE; i++) {
        fprintf(file, "%c%c", game->players[PLAYERONE].hand[i].rank, 
            game->players[PLAYERONE].hand[i].suit);
    }
    fprintf(file, "\n");

    for (int i = 0; i < HANDSIZE; i++) {
        fprintf(file, "%c%c", game->players[PLAYERTWO].hand[i].rank, 
            game->players[PLAYERTWO].hand[i].suit);
    }
    fprintf(file, "\n");

    for (int i = 0; i < game->height; i++) {
        for (int j = 0; j < game->width; j++) {
            if (game->board[i][j].rank == EMPTYBOARDSPACE && 
                    game->board[i][j].suit == EMPTYBOARDSPACE) {
                fprintf(file, "%c%c", EMPTYFILEBDSPACE, EMPTYFILEBDSPACE);
            } else {
                fprintf(file, "%c%c", 
                    game->board[i][j].rank, game->board[i][j].suit);
            }
        }
        if (i < game->height - 1) {
            fprintf(file, "\n");
        }
    }

    fclose(file);
}

/**
 * Handles logic for a human move.
 * Params: Game struct, player number int.
 * Returns: nothing (void).
 */
Status human_move(Game* game, int player) {
    game->playerTurn = player;

    print_board(game);

    draw_card(game, player, game->players[player].prevCardPos);

    print_hand(game, player);

    return get_move(game, player);
}

/**
 * Handles logic for an automated move.
 * Params: Game struct, player number int.
 * Returns: nothing (void).
 */
void auto_move(Game* game, int player) {
    game->playerTurn = player;

    print_board(game);

    draw_card(game, player, game->players[player].prevCardPos);

    print_hand(game, player);
    
    // Auto move logic.
    int x;
    int y;

    if (game->spacesFilled == 0) {
        x = game->width / 2;
        y = game->height / 2;
    } else {
        switch (player) {
            case PLAYERONE:
                for (int i = 0; i < game->height; i++) {
                    for (int j = 0; j < game->width; j++) {
                        if (valid_move(game, 1, i + 1, j + 1)) {
                            x = j + 1;
                            y = i + 1;
                        }
                    }
                }
                break;
            case PLAYERTWO:
                for (int i = game->height - 1; i > -1; i--) {
                    for (int j = game->width - 1; j > -1; j--) {
                        if (valid_move(game, 1, i + 1, j + 1)) {
                            x = j + 1;
                            y = i + 1;
                        }
                    }
                }
                break;
            default:
                break;
        }
    }

    game->board[y - 1][x - 1].rank 
            = game->players[player].hand[0].rank;
    game->board[y - 1][x - 1].suit
        = game->players[player].hand[0].suit;
    game->spacesFilled += 1;
    game->players[player].prevCardPos = 0;

    // Print move to stdout.
    sleep(1);
    printf("Player %d plays %c%c in column %d row %d\n", player + 1, 
        game->players[player].hand[0].rank, 
        game->players[player].hand[0].suit, x, y);
}

/**
 * Runs the main logic of the game.
 * Params: Game struct.
 * Returns: a status depending what happens.
 */
Status game_loop(Game* game) {
    fill_hand(game, PLAYERONE);
    fill_hand(game, PLAYERTWO);

    Status move;

    while (1) {
        if (game->spacesFilled == game->width * game->height || 
                game->deck.numCards - (game->deck.cardsPlayed + 1) 
                < MINDECKSIZE) {
            break;
        }
        
        if (game->playerTurn % 2 == PLAYERONE) {
            switch (game->players[PLAYERONE].type) {
                case HUMAN:
                    move = human_move(game, PLAYERONE);
                    break;
                case AUTO:
                    auto_move(game, PLAYERONE);
                    break;
            }
            game->playerTurn += 1;
        } else {
            switch (game->players[PLAYERTWO].type) {
                case HUMAN:
                    move = human_move(game, PLAYERTWO);
                    break;
                case AUTO:
                    auto_move(game, PLAYERTWO);
                    break;
            }
            game->playerTurn += 1; 
        } 
    }

    if (move != NORMAL) {
        return move;
    }

    cal_scores(game);
    free(game->board);
    free(game->deck.cards);

    return NORMAL;
}

/**
 * Is the card at the specified location of higher rank than the rank parameter.
 * Params: game struct, card rank, int for column and row.
 * Returns: true if cell is empty or contains a higher ranked card.
 */
bool is_greater(Game* game, char rank, int col, int row) {
    return (game->board[row][col].rank != EMPTYBOARDSPACE)
            && (game->board[row][col].rank > rank);
}

/**
 * Calculate the maximum points possible for a path travelling through a cell.
 * Params: game struct, column and row number, suit char.
 * Returns: an int score.
 */
int score_pos(Game* game, int col, int row, char suit) {
    int width = game->width;
    int height = game->height;
    Card current = game->board[row][col];
    int total = (current.suit == suit ? 1 : 0);

    // Right direction
    if (is_greater(game, current.rank, (col + width - 1) % width, row)) {
        int result = score_pos(game, (col + width - 1) % width, row, suit);
        if ((result != 0) && (result + 1 > total)) {
            total = result + 1;
        }
    }

    // Left direction
    if (is_greater(game, current.rank, (col + 1) % width, row)) {
        int result = score_pos(game, (col + 1) % width, row, suit);
        if ((result != 0) && (result + 1 > total)) { 
            total = result + 1;
        }
    }

    // Down direction
    if (is_greater(game, current.rank, col, (row + 1) % height)) {
        int result = score_pos(game, col, (row + 1) % height, suit);
        if ((result != 0) && (result + 1 > total)) { 
            total = result + 1;
        }
    }

    // Up direction
    if (is_greater(game, current.rank, col, (row + height - 1) % height)) {
        int result = score_pos(game, col, (row + height - 1) % height, suit);
        if ((result != 0) && (result + 1 > total)) { 
            total = result + 1;
        }
    }

    return total;
}

/**
 * Calculates the scores for the players at the end of a game.
 * Params: Game struct.
 * Returns: nothing (void).
 */
void cal_scores(Game* game) {
    // Calculate scores logic.
    for (int i = 0; i < game->height; i++) {
        for (int j = 0; j < game->width; j++) {
            if (game->board[i][j].rank != EMPTYBOARDSPACE && 
                    game->board[i][j].suit != EMPTYBOARDSPACE) {
                char suit = game->board[i][j].suit;
                int score = score_pos(game, j, i, suit);
                if (game->players[suit - 'A' % 2].score < score) {
                    game->players[suit - 'A' % 2].score = score;
                }
            }
        }
    }

    // Print scores to stdout.
    printf("Player 1=%d Player 2=%d\n", 
        game->players[PLAYERONE].score, game->players[PLAYERTWO].score);
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

    return show_message(game_loop(&game));
}

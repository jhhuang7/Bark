#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Function definitions
int dimensions_valid_in(int width, int height, char** wend, char** hend);
int ptype_valid_in(char** p1type, char** p2type);
void print_board(char**** board, int width, int height);
void empty_board(char**** board, int width, int height);
int get_line_input(char** cmd, char** file);
int human_move(int pl, char*** hand, char*** hand2, int w, int h, char**** bd, 
        char*** deck, int currentCard, char* deckfile, int spacesFilled);
void auto_move(int type, int pl, char*** phand, int w, int h, 
        char**** board, char*** deck, int currentCard, int spaces);
int no_cards_in_deck(char* filename);
void put_cards_in_deck(char* filename, char*** deck);
int fill_player_hand(char*** phand, char*** deck, int currentCard);
void refill_hand(char*** phand, char*** deck, int playedPos, int currentCard);
int first_line_savefile(char* filename, int rtnNum);
void saved_deckfile(char* filename, char** deckfile);
void saved_hands(char* filename, char*** p1hand, char*** p2hand);
void saved_board(char* filename, char**** board, int width, int height);
int calc_score(int player, char**** board, int width, int height);
int val_mov(char**** board, int xcord, int ycord, int width, 
        int height, int spacesFilled);
int val_in(int cardPos, int xcord, int ycord, 
        int width, int height);
void save_game(char**** board, char*** p1hand, char*** p2hand, char* savefile, 
        char* deckfile, int width, int height, 
        int cardsPlayed, int playerTurn);

/**
 * Main fuction of program
 */
int main(int argc, char** argv) {
    char* deckfile;
    char* savefile;
    int width;
    int height;
    char* p1type;
    char* p2type;

    if (argc == 6) {
        deckfile = argv[1];
        char* wend;
        char* hend;
        width = strtol(argv[2], &wend, 10);
        height = strtol(argv[3], &hend, 10);
        p1type = argv[4];
        p2type = argv[5];
        
        // Player types are not a or h or the board dimensions are not integers
        // in the correct range
        if ((dimensions_valid_in(width, height, (char**)&wend, 
                (char**)&hend) == 2)
                || (ptype_valid_in((char**)&p1type, (char**)&p2type) == 2)) {
            fprintf(stderr, "Incorrect arg types\n");
            return 2;
        }

        int noCards = no_cards_in_deck(deckfile);
        int boardSpace = width * height;
        
        // Can’t read/process deckfile (in either mode) This includes errors
        // in the deckfile format
        if (noCards == -3) {
            fprintf(stderr, "Unable to parse deckfile\n");
            return 3;
        }

        // Not enough cards left in deck for a new game
        if (noCards < 11) {
            fprintf(stderr, "Short deck\n");
            return 5;
        }

        // Creates an empty board
        char*** board = malloc(sizeof(char**) * height);
        empty_board((char****)&board, width, height);
        
        // Prints the board
        print_board((char****)&board, width, height);

        // Turns deckfile into an array
        char** deck = malloc(sizeof(char*) * (noCards + 1));
        put_cards_in_deck(deckfile, (char***)&deck);
        
        // Fills the players' hands
        char** p1hand = malloc(sizeof(char*) * 6);
        char** p2hand = malloc(sizeof(char*) * 6);
        int currentCard = 1;   

        currentCard = fill_player_hand((char***)&p1hand, 
                (char***)&deck, currentCard);
        currentCard = fill_player_hand((char***)&p2hand, 
                (char***)&deck, currentCard);

        // Allows the players to input onto the board
        int spacesFilled = 0;
        int saved = 0;

        while (currentCard <= noCards && spacesFilled <= boardSpace) {
            if (strcmp(p1type, "h") + strcmp(p2type, "h") == 0) {
                if (currentCard == noCards + 1 || spacesFilled == boardSpace) {
                    break;
                }

                // Handles player 1 input
                saved = human_move(1, (char***)&p1hand, (char***)&p2hand, 
                        width, height, (char****)&board, (char***)&deck, 
                        currentCard, deckfile, spacesFilled);

                // End of input for human player
                if (saved == 7) {
                    fprintf(stderr, "End of input\n");
                    return 7;
                }

                if (saved == 0) {
                    currentCard += 2;
                    print_board((char****)&board, width, height);
                    spacesFilled++;
                }

                if (currentCard == noCards + 1 || spacesFilled == boardSpace) {
                    break;
                }
                
                // Handles player 2 input
                saved = human_move(2, (char***)&p2hand, (char***)&p1hand, 
                        width, height, (char****)&board, (char***)&deck, 
                        currentCard, deckfile, spacesFilled);

                // End of input for human player
                if (saved == 7) {
                    fprintf(stderr, "End of input\n");
                    return 7;
                }

                if (saved == 0) {
                    currentCard += 2; 
                    print_board((char****)&board, width, height);
                    spacesFilled++;
                }
            }

            if (strcmp(p1type, "a") + strcmp(p2type, "a") == 0) {
                if (currentCard == noCards + 1 || spacesFilled == boardSpace) {
                    break;
                }

                auto_move(0, 1, (char***)&p1hand, width, 
                        height, (char****)&board, (char***)&deck, 
                        currentCard, spacesFilled);
                currentCard += 2;
                print_board((char****)&board, width, height);
                spacesFilled++;

                if (currentCard == noCards + 1 || spacesFilled == boardSpace) {
                    break;
                }

                auto_move(0, 2, (char***)&p2hand, width, 
                        height, (char****)&board, (char***)&deck, 
                        currentCard, spacesFilled);
                currentCard += 2;
                print_board((char****)&board, width, height);
                spacesFilled++;
            }

            if (strcmp(p1type, "h") + strcmp(p2type, "a") == 0) {
                if (currentCard == noCards + 1 || spacesFilled == boardSpace) {
                    break;
                }

                // Handles player 1 input
                saved = human_move(1, (char***)&p1hand, (char***)&p2hand, 
                        width, height, (char****)&board, (char***)&deck, 
                        currentCard, deckfile, spacesFilled);

                // End of input for human player
                if (saved == 7) {
                    fprintf(stderr, "End of input\n");
                    return 7;
                }

                if (saved == 0) {
                    currentCard += 2;
                    print_board((char****)&board, width, height);
                    spacesFilled++;
                }

                if (currentCard == noCards + 1 || spacesFilled == boardSpace) {
                    break;
                }

                auto_move(1, 2, (char***)&p2hand, width, 
                        height, (char****)&board, (char***)&deck, 
                        currentCard, spacesFilled);
                currentCard += 2;
                print_board((char****)&board, width, height);
                spacesFilled++;
            }

            if (strcmp(p1type, "a") + strcmp(p2type, "h") == 0) {
                if (currentCard == noCards + 1 || spacesFilled == boardSpace) {
                    break;
                }

                auto_move(1, 1, (char***)&p1hand, width, 
                        height, (char****)&board, (char***)&deck, 
                        currentCard, spacesFilled);
                currentCard += 2;
                print_board((char****)&board, width, height);
                spacesFilled++;

                if (currentCard == noCards + 1 || spacesFilled == boardSpace) {
                    break;
                }
                
                // Handles player 2 input
                saved = human_move(2, (char***)&p2hand, (char***)&p1hand, 
                        width, height, (char****)&board, (char***)&deck, 
                        currentCard, deckfile, spacesFilled);

                // End of input for human player
                if (saved == 7) {
                    fprintf(stderr, "End of input\n");
                    return 7;
                }

                if (saved == 0) {
                    currentCard += 2; 
                    print_board((char****)&board, width, height);
                    spacesFilled++;
                }
            }
        }
         
        printf("Player 1=%d Player 2=%d\n", 
                calc_score(1, (char****)&board, width, height), 
                calc_score(2, (char****)&board, width, height));
        
    } else if (argc == 4) {
        savefile = argv[1];
        p1type = argv[2];
        p2type = argv[3];

        if (ptype_valid_in((char**)&p1type, (char**)&p2type) == 2) {
            fprintf(stderr, "Incorrect arg types\n");
            return 2;
        }

        // Can’t read/process savefile
        if (first_line_savefile(savefile, 1) == -4) {
            fprintf(stderr, "Unable to parse savefile\n");
            return 4;
        }

        width = first_line_savefile(savefile, 1);
        height = first_line_savefile(savefile, 2);
        int currentCard = first_line_savefile(savefile, 3) + 1;
        int playerTurn = first_line_savefile(savefile, 4);
        int boardSpace = width * height;

        // Board full on load
        if (currentCard - 11 >= boardSpace) {
            fprintf(stderr, "Board full\n");
            return 6;
        }

        // Turns deckfile into an array
        deckfile = malloc(sizeof(char) * 100);
        saved_deckfile(savefile, (char**)&deckfile);

        int noCards = no_cards_in_deck(deckfile);

        // Can’t read/process deckfile (in either mode)
        // This includes errors in the deckfile format
        if (noCards == -3) {
            fprintf(stderr, "Unable to parse deckfile\n");
            return 3;
        }

        // Not enough cards left in deck for a new game
        if (noCards < 11) {
            fprintf(stderr, "Short deck\n");
            return 5;
        }

        // Turns deckfile into an array
        char** deck = malloc(sizeof(char*) * (noCards + 1));
        put_cards_in_deck(deckfile, (char***)&deck);
        
        // Fills the players' hands
        char** p1hand = malloc(sizeof(char*) * 6);
        char** p2hand = malloc(sizeof(char*) * 6);

        saved_hands(savefile, (char***)&p1hand, (char***)&p2hand);

        // Creates an empty board from given width and height and prints it
        char*** board = malloc(sizeof(char**) * height);
        empty_board((char****)&board, width, height);
        saved_board(savefile, (char****)&board, width, height);
        print_board((char****)&board, width, height);

        // Allows the players to input onto the board
        int spacesFilled = 0;
        int saved = 0;

        while (currentCard <= noCards && spacesFilled <= boardSpace) {
            if (strcmp(p1type, "h") + strcmp(p2type, "h") == 0) {
                if (currentCard == noCards + 1 || spacesFilled == boardSpace) {
                    break;
                }

                // Handles player 1 input
                if (playerTurn == 1) {
                    saved = human_move(1, (char***)&p1hand, (char***)&p2hand, 
                            width, height, (char****)&board, (char***)&deck, 
                            currentCard, deckfile, spacesFilled);
                } else {
                    saved = human_move(2, (char***)&p2hand, (char***)&p1hand, 
                            width, height, (char****)&board, (char***)&deck, 
                            currentCard, deckfile, spacesFilled);
                }

                // End of input for human player
                if (saved == 7) {
                    fprintf(stderr, "End of input\n");
                    return 7;
                }

                if (saved == 0) {
                    currentCard += 2;
                    print_board((char****)&board, width, height);
                    spacesFilled++;
                }

                if (currentCard == noCards + 1 || spacesFilled == boardSpace) {
                    break;
                }
                
                // Handles player 2 input
                if (playerTurn == 1) {
                    saved = human_move(2, (char***)&p2hand, (char***)&p1hand, 
                            width, height, (char****)&board, (char***)&deck, 
                            currentCard, deckfile, spacesFilled);
                } else {
                    saved = human_move(1, (char***)&p1hand, (char***)&p2hand, 
                            width, height, (char****)&board, (char***)&deck, 
                            currentCard, deckfile, spacesFilled);
                }

                // End of input for human player
                if (saved == 7) {
                    fprintf(stderr, "End of input\n");
                    return 7;
                }

                if (saved == 0) {
                    currentCard += 2;
                    print_board((char****)&board, width, height);
                    spacesFilled++;
                }
            }

            if (strcmp(p1type, "a") + strcmp(p2type, "a") == 0) {
                if (currentCard == noCards + 1 || spacesFilled == boardSpace) {
                    break;
                }

                if (playerTurn == 1) {
                    auto_move(0, 1, (char***)&p1hand, width, 
                            height, (char****)&board, (char***)&deck, 
                            currentCard, spacesFilled);
                } else {
                    auto_move(0, 2, (char***)&p2hand, width, 
                            height, (char****)&board, (char***)&deck, 
                            currentCard, spacesFilled);
                }
                currentCard += 2;
                print_board((char****)&board, width, height);
                spacesFilled++;

                if (currentCard == noCards + 1 || spacesFilled == boardSpace) {
                    break;
                }

                if (playerTurn == 1) {
                    auto_move(0, 2, (char***)&p2hand, width, 
                            height, (char****)&board, (char***)&deck, 
                            currentCard, spacesFilled);
                } else {
                    auto_move(0, 1, (char***)&p1hand, width, 
                            height, (char****)&board, (char***)&deck, 
                            currentCard, spacesFilled);
                }
                currentCard += 2;
                print_board((char****)&board, width, height);
                spacesFilled++;
            }

            if (strcmp(p1type, "h") + strcmp(p2type, "a") == 0) {
                if (currentCard == noCards + 1 || spacesFilled == boardSpace) {
                    break;
                }

                // Handles player 1 input
                if (playerTurn == 1) {
                    saved = human_move(1, (char***)&p1hand, (char***)&p2hand, 
                            width, height, (char****)&board, (char***)&deck, 
                            currentCard, deckfile, spacesFilled);

                    // End of input for human player
                    if (saved == 7) {
                        fprintf(stderr, "End of input\n");
                        return 7;
                    }

                    if (saved == 0) {
                        currentCard += 2;
                        print_board((char****)&board, width, height);
                        spacesFilled++;
                    }

                    if (currentCard == noCards + 1 
                            || spacesFilled == boardSpace) {
                        break;
                    }

                    auto_move(1, 2, (char***)&p2hand, width, 
                            height, (char****)&board, (char***)&deck, 
                            currentCard, spacesFilled);
                    currentCard += 2;
                    print_board((char****)&board, width, height);
                    spacesFilled++;

                } else {
                    auto_move(1, 1, (char***)&p1hand, width, 
                            height, (char****)&board, (char***)&deck, 
                            currentCard, spacesFilled);
                    currentCard += 2;
                    print_board((char****)&board, width, height);
                    spacesFilled++;

                    saved = human_move(2, (char***)&p2hand, (char***)&p1hand, 
                            width, height, (char****)&board, (char***)&deck, 
                            currentCard, deckfile, spacesFilled);

                    // End of input for human player
                    if (saved == 7) {
                        fprintf(stderr, "End of input\n");
                        return 7;
                    }

                    if (saved == 0) {
                        currentCard += 2;
                        print_board((char****)&board, width, height);
                        spacesFilled++;
                    }
                }
            }

            if (strcmp(p1type, "a") + strcmp(p2type, "h") == 0) {
                if (currentCard == noCards + 1 || spacesFilled == boardSpace) {
                    break;
                }

                if (playerTurn == 1) {
                    auto_move(1, 1, (char***)&p1hand, width, 
                            height, (char****)&board, (char***)&deck, 
                            currentCard, spacesFilled);
                    currentCard += 2;
                    print_board((char****)&board, width, height);
                    spacesFilled++;

                    saved = human_move(2, (char***)&p2hand, (char***)&p1hand, 
                            width, height, (char****)&board, (char***)&deck, 
                            currentCard, deckfile, spacesFilled);

                    // End of input for human player
                    if (saved == 7) {
                        fprintf(stderr, "End of input\n");
                        return 7;
                    }

                    if (saved == 0) {
                        currentCard += 2;
                        print_board((char****)&board, width, height);
                        spacesFilled++;
                    }

                    if (currentCard == noCards + 1 
                            || spacesFilled == boardSpace) {
                        break;
                    }

                } else {
                    saved = human_move(1, (char***)&p1hand, (char***)&p2hand, 
                            width, height, (char****)&board, (char***)&deck, 
                            currentCard, deckfile, spacesFilled);

                    // End of input for human player
                    if (saved == 7) {
                        fprintf(stderr, "End of input\n");
                        return 7;
                    }

                    if (saved == 0) {
                        currentCard += 2;
                        print_board((char****)&board, width, height);
                        spacesFilled++;
                    }

                    auto_move(1, 2, (char***)&p2hand, width, 
                            height, (char****)&board, (char***)&deck, 
                            currentCard, spacesFilled);
                    currentCard += 2;
                    print_board((char****)&board, width, height);
                    spacesFilled++;
                }
            }
        }

        printf("Player 1=%d Player 2=%d\n", 
                calc_score(1, (char****)&board, width, height), 
                calc_score(2, (char****)&board, width, height));
    }

    // Incorrect number of args
    if (argc != 4 && argc != 6) {
        fprintf(stderr, "Usage: bark savefile p1type p2type\n");
        fprintf(stderr, "bark deck width height p1type p2type\n");
        return 1;
    }
   
    // Normal game over
    fflush(stdout);
    return 0;
}

/**
 * Check input dimenstions are valid
 */
int dimensions_valid_in(int width, int height, char** wend, char** hend) {
    if ((strcmp(*wend, "") != 0) || (strcmp(*hend, "") != 0)
            || (width < 3) || (width > 100) || 
            (height < 3) || (height > 100)) {
        return 2;
    }
    return 0;
}

/**
 * Check player type inputs are valid
 */
int ptype_valid_in(char** p1type, char** p2type) {
    if (((strcmp(*p1type, "h") != 0) && (strcmp(*p1type, "a") != 0))
            || ((strcmp(*p2type, "h") != 0) && (strcmp(*p2type, "a") != 0))) {
        return 2;
    }
    return 0;
}

/**
 * Makes an empty board with dots
 */
void empty_board(char**** board, int width, int height) {
    for (int i = 0; i < height; i++) {
        (*board)[i] = malloc(sizeof(char*) * width);
        for (int j = 0; j < width; j++) {
            (*board)[i][j] = malloc(sizeof(char) * 3);
            (*board)[i][j] = "..";
        }
    }
}

/**
 * Prints the board based on given width and height
 */
void print_board(char**** board, int width, int height) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            printf("%s", (*board)[i][j]);
        }
        printf("\n");
    }
}

/**
 * Turns deckfile into an array
 */
void put_cards_in_deck(char* filename, char*** deck) {
    int i = 0;
    
    char line[50];

    FILE* file;
    file = fopen(filename, "r");

    while(fgets(line, sizeof line, file) != NULL) {
        (*deck)[i] = malloc(sizeof(line));
        strcpy((*deck)[i], line);
        i++;
    }

    fclose(file);
}

/**
 * Gets line from humna input
 */
int get_line_input(char** cmd, char** file) {
    printf("Move? ");

    int c, i = 0;

    do {
        c = fgetc(stdin);

        if (feof(stdin)) { 
            return 7;
        }

        if (c == '\n') {
            break;
        }

        (*cmd)[i] = (char)c;

        if (i >= 4) {
            (*file)[i - 4] = (char)c;
        }

        i++;

    } while(1);

    return 0;
}

/**
 * Handles human vs human player input
 */
int human_move(int pl, char*** hand, char*** hand2, int w, int h, char**** bd, 
        char*** deck, int currentCard, char* deckfile, int s) {
    int savedGame = 0;
    (*hand)[5] = (*deck)[currentCard];
    currentCard++;
    printf("Hand(%d): ", pl);
    for (int a = 0; a < 6; a++) {
        (a < 5) ? printf("%s ", strtok((*hand)[a], "\n")) :
                printf("%s", strtok((*hand)[a], "\n"));
    }
    printf("\n");
    int c, x, y;
    while (1) {
        char* cmd = malloc(100);        
        char* file = malloc(100);
        int getInput = get_line_input((char**)&cmd, (char**)&file);
        if (getInput == 7) { 
            return 7;
        }
        int len = strlen(cmd);
        cmd[len] = '\0';
        file[len - 4] = '\0';
        if (cmd[0] == 'S' && cmd[1] == 'A' && cmd[2] == 'V' && cmd[3] == 'E') {
            if (len >= 5) {
                if (pl == 1) {
                    save_game((char****)&bd, (char***)&hand, (char***)&hand2, 
                            file, deckfile, w, h, currentCard, 1);
                } else {
                    save_game((char****)&bd, (char***)&hand2, (char***)&hand, 
                            file, deckfile, w, h, currentCard, 2);
                }
                savedGame = 1;
                break;
            } else {
                printf("Unable to save\n");
                continue;
            }
        } else {
            sscanf(cmd, "%d %d %d ", &c, &x, &y);
            if (val_in(c, x, y, w, h) && val_mov(bd, x, y, w, h, s)) {         
                break;
            } else {
                continue;
            }
        }
    }
    if (savedGame == 0) {
        (*bd)[y - 1][x - 1] = (*hand)[c - 1];
        refill_hand(hand, deck, c, currentCard);
    }
    return savedGame;
}

/**
 * Makes move for automated player
 */
void auto_move(int type, int pl, char*** phand, int w, int h, 
        char**** board, char*** deck, int currentCard, int spaces) {
    (*phand)[5] = (*deck)[currentCard];
    currentCard++;
    if (type == 1) {
        printf("Hand(%d): ", pl);
    } else {
        printf("Hand: ");
    }
    for (int a = 0; a < 6; a++) {
        (a < 5) ? printf("%s ", strtok((*phand)[a], "\n")) :
                printf("%s", strtok((*phand)[a], "\n"));
    }
    printf("\n");
    int x, y;
    int xmid = w / 2 + w % 2, ymid = h / 2 + h % 2;
    if (spaces == 0) {
        (*board)[ymid - 1][xmid - 1] = (*phand)[0];
        x = xmid;
        y = ymid;
    } else {
        if (pl == 1) {
            for (int i = 0; i < h; i++) {
                for (int j = 0; j < w; j++) {
                    if (val_mov(board, i + 1, j + 1, w, h, spaces)) {
                        (*board)[i][j] = (*phand)[0];
                        x = j + 1;
                        y = i + 1;
                        break;
                    }
                    continue;
                }
                break;
            }
        } else {
            for (int i = h - 1; i > 0; i--) {
                for (int j = w - 1; j > 0; j--) {
                    if (val_mov(board, i + 1, j + 1, w, h, spaces)) {
                        (*board)[i][j] = (*phand)[0];
                        x = j + 1;
                        y = i + 1;
                        break;
                    }
                    continue;
                }
                break;
            }
        }
    }
    printf("Player %d plays %s in column %d row %d\n", pl, (*phand)[0], x, y);
    refill_hand(phand, deck, 0, currentCard);
}

/**
 * Read first line of deckfile to see how many cards are in the deck
 */
int no_cards_in_deck(char* filename) {
    FILE* file;
    char line[1000];

    file = fopen(filename, "r");

    if (file == NULL) {
        // Program exits if file pointer returns NULL.
        return -3;
    }

    // reads text until newline 
    fscanf(file, "%[^\n]", line);

    fclose(file);
    
    return atoi(line);
}

/**
 * Fills a given player's hands
 */
int fill_player_hand(char*** phand, char*** deck, int currentCard) {
    for (int j = 0; j < 5; j++) {
        (*phand)[j] = (*deck)[currentCard];
        currentCard++;
    }
    return currentCard;
}

/**
 * Refills player's hand after move
 */
void refill_hand(char*** phand, char*** deck, int playedPos, int currentCard) {
    for (int i = playedPos - 1; i < 5; i++) {
        (*phand)[i] = (*phand)[i + 1];
    }
}

/**
 * Reads first line of savefile
 */
int first_line_savefile(char* filename, int rtnNum) {
    FILE* file;

    char line[100];
    int width;
    int height;
    int cardsPlayed;
    int playerTurn;
    
    if ((file = fopen(filename, "r")) == NULL) {
        // Program exits if file pointer returns NULL.
        return -4;         
    }

    fscanf(file, "%[^\n]", line);
    width = atoi(strtok(line, " "));
    height = atoi(strtok(NULL, " "));
    cardsPlayed = atoi(strtok(NULL, " "));
    playerTurn = atoi(strtok(NULL, " "));

    fclose(file);

    switch(rtnNum) {
        case 1:
            return width;
            break;
        case 2:
            return height;
            break;
        case 3:
            return cardsPlayed;
            break;
        case 4:
            return playerTurn;
            break;
        default:
            return -4;
    }
}

/**
 * Obtains the deckfile name from savefile
 */
void saved_deckfile(char* filename, char** deckfile) {
    FILE* file;

    char line1[100];
    char* deckname = malloc(100);
    
    file = fopen(filename, "r");

    fscanf(file, "%[^\n]", line1);
    fscanf(file, "%s", deckname);

    strcpy(*deckfile, deckname);

    fclose(file);
}

/**
 * Reads savefile and fills players' hands
 */
void saved_hands(char* filename, char*** p1hand, char*** p2hand) {
    FILE* file;

    char line1[100];
    char line2[100];
    char* hand1 = malloc(sizeof(char) * 12);
    char* hand2 = malloc(sizeof(char) * 12);

    file = fopen(filename, "r");

    fscanf(file, "%[^\n] %[^\n] %s %s", line1, line2, hand1, hand2);

    for (int i = 0; i < 12; i += 2) {
        char* card = malloc(3);
        strncpy(card, &hand1[i], 2);
        card[2] = '\0';
        (*p1hand)[i / 2] = card;
    }

    for (int i = 0; i < 12; i += 2) {
        char* card = malloc(3);
        strncpy(card, &hand2[i], 2);
        card[2] = '\0';
        (*p2hand)[i / 2] = card;
    }

    fclose(file);
}

/**
 * Loads the board from savefile
 */
void saved_board(char* filename, char**** board, int width, int height) {
    FILE* file;

    char line1[100];
    char line2[100];
    char line3[100];
    char line4[100];
    
    file = fopen(filename, "r");

    fscanf(file, "%[^\n] %[^\n] %[^\n] %[^\n]", line1, line2, line3, line4);

    char* bd = malloc(20000);
    char* bd1 = malloc(20000);
    
    for (int i = 0; i < height; i++) {
        fscanf(file, "%s", bd);
        strcat(bd1, bd);    
    }

    int i = 0;

    for (int j = 0; j < (width * height); j += 2) {
        if (j % (width * 2) == 0) {
            i++;
        }

        char* card = malloc(3);
        strncpy(card, &bd1[j], 2);
        card[2] = '\0';

        if (strcmp(card, "**") != 0) {
            int y = i - 1;
            int x = j / 2 - width * (i - 1);
            (*board)[y][x] = card;
        }
    }

    free(bd);
    free(bd1);
}

/**
 * Calculates score for players based on board
 */
int calc_score(int player, char**** board, int width, int height) {
    if (player == 1) {
        return 1 + width;
    } else {
        return 2 + height;
    }
}

/**
 * Check if move made is valid
 */
int val_mov(char**** board, int xcord, int ycord, int width, 
        int height, int spacesFilled) {
    if (spacesFilled == 0) {
        return 1;
    }

    int x = xcord - 1;
    int y = ycord - 1;

    int t = y - 1;
    int l = x - 1;
    int d = y + 1;
    int r = x + 1;

    if (t == -1) {
        t = height - 1;
    }
    if (l == -1) {
        l = width - 1;
    }
    if (d == height) {
        d = 0;
    }
    if (r == width) {
        r = 0;
    }

    if ((strcmp((*board)[y][x], "..") == 0) && 
            ((strcmp((*board)[t][x], "..") != 0) || 
            (strcmp((*board)[y][l], "..") != 0) || 
            (strcmp((*board)[d][x], "..") != 0) || 
            (strcmp((*board)[y][r], "..") != 0))) {
        return 1;
    }

    return 0;
}

/**
 * Check if move input integers are in range
 */
int val_in(int cardPos, int xcord, int ycord, 
        int width, int height) {
    if ((cardPos >= 1) && (cardPos <= 6) && (xcord >= 1) &&
            (xcord <= width) && (ycord >= 1) && (ycord <= height)) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Saves game
 */
void save_game(char**** board, char*** p1hand, char*** p2hand, char* savefile, 
        char* deckfile, int width, int height, 
        int cardsPlayed, int playerTurn) {
    FILE* file;

    file = fopen(savefile, "w");

    fprintf(file, "%d %d %d %d\n", width, height, cardsPlayed, playerTurn);

    fprintf(file, "%s\n", deckfile);

    for (int i = 0; i < 6; i++) {
        fprintf(file, "%s", strtok((*p1hand)[i], "\n"));
    }
    fprintf(file, "\n");

    for (int i = 0; i < 6; i++) {
        fprintf(file, "%s", strtok((*p2hand)[i], "\n"));
    }
    fprintf(file, "\n");

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if ((strcmp((*board)[i][j], "..") == 0)) {
                fprintf(file, "**");
            } else {
                fprintf(file, "%s", (*board)[i][j]);
            }
        }
        if (i < height - 1) {
            fprintf(file, "\n");
        }
    }

    fclose(file);
}
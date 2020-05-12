# Introduction
The game consists of a deck of cards and a board; and can be played with either 2 human or automated players. The deck consists of cards each having a number between 1 and 9 and a suit represented by a capital letter.

A new game will end immediately if there are not at least 11 cards in the deck. Each player draws a starting hand of 5 cards from the deck. At the beginning of each player’s turn, they draw one card from the deck and then play one card onto the board in an unoccupied space.

Each card played (except for the first one) must be played immediately adjacent (horizontally or vertically; for the purposes of adjacency, the opposite edges of the board wrap) to a card already on the board.

The game ends at the end of the turn when either: the last card has been drawn from the deck or there is no free space on the board. At the end of the game, the player with the longest path wins. For each suit, find the longest path between two cards of the same suit. Each step in the path: must be from a smaller number to a bigger number, and must horizontal or vertical.

# Game Instructions
## Invocation
To start a new game: "./bark deckfile width height p1type p2type"
To load a saved game: "./bark savefile p1type p2type"
deckfile/savefile are the names of / paths to the files.
The acceptable player types are: h or a.
h denotes a player whose moves will come via stdin.
a denotes a player whose moves will be chosen by the program.
The dimensions (width and height) of the board must each be between 3 and 100 inclusive.

## Interaction
To place a card from hand: "card c r"
Where card is a number between 1 and 6 (inclusive) indicating the card from hand to play; and c and r are the
column and row respectively (beginning with 1). If the move is not valid (including if the place indicated by
(c, r) is already filled), a new move is prompted.

To save a game: "SAVEzzzzz"
Where zzzzz is the name of the file to save the game to. This name must contain at least one character.

Automated players will not prompt for moves. Instead, after they have chosen a move, they will print their move to stdout.

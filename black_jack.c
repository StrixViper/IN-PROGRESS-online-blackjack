#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_NAME_LEN 50

typedef enum
{
    ACE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,  // Fixed typo
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING
} VALUE;

typedef enum
{
    HEARTS,
    DIAMONDS,
    SPADES,
    CLUBS
} SUIT;

typedef enum
{
    RED,
    BLACK
} COLOR;

typedef struct
{
    VALUE Value;
    SUIT Suit;
    COLOR Color;
} Card;

typedef struct
{
    Card card[2];
    char name[MAX_NAME_LEN];
    double ChipSum;
} Player;

typedef struct
{
    Card* cards;      // Now a pointer to a dynamically allocated array of Cards
} Deck;

typedef struct
{
    Deck* deck;
    Card dealerCards[2];
    double sumBetting;
} Board;

typedef struct
{
    Player* players;  // Pointer to a dynamically allocated array of Players
    Board* board;
} Game;


void initializePlayer(Player* player);
void initializeBoard(Board* board);
void initializeDeck(Deck* deck);
void shuffleDeck(Deck* deck);
void freeGame(Game* game);

void dealCards(Game* game);
void printCards(Card* card);

int main()
{
    return 0;
}

void initializePlayer(Player* player) {
    player->ChipSum = 250.0;
    strcpy(player->name, "Default Name");  // Optional: set a default name
}

void initializeDeck(Deck* deck) {
    deck->cards = malloc(52 * sizeof(Card));  // Allocate space for 52 cards
    if (deck->cards == NULL) {
        perror("Failed to allocate memory for deck cards");
        exit(EXIT_FAILURE);
    }

    SUIT suits[4] = {HEARTS, DIAMONDS, SPADES, CLUBS};
    VALUE values[13] = {ACE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING};

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 13; j++) {
            int index = i * 13 + j;
            deck->cards[index].Suit = suits[i];
            deck->cards[index].Value = values[j];
            deck->cards[index].Color = (suits[i] == HEARTS || suits[i] == DIAMONDS) ? RED : BLACK;
        }
    }
}

void initializeBoard(Board* board) {
    board->deck = malloc(sizeof(Deck));
    if (board->deck == NULL) {
        perror("Failed to allocate memory for deck");
        exit(EXIT_FAILURE);
    }
    initializeDeck(board->deck);  // Initialize the deck within the board

    board->sumBetting = 0.0;  // Initialize the betting sum to zero
}

void initializeGame(Game* game, int playerCount) {
    game->board = malloc(sizeof(Board));
    if (game->board == NULL) {
        perror("Failed to allocate memory for board");
        exit(EXIT_FAILURE);
    }
    initializeBoard(game->board);  // Initialize the board

    game->players = malloc(playerCount * sizeof(Player));
    if (game->players == NULL) {
        perror("Failed to allocate memory for players");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < playerCount; i++) {
        initializePlayer(&game->players[i]);
    }
}

void shuffleDeck(Deck* deck) {
    // Seed the random number generator
    srand((unsigned) time(NULL));

    for (int i = 51; i > 0; i--) {
        // Generate a random index between 0 and i
        int j = rand() % (i + 1);

        // Swap deck->card[i] with deck->card[j]
        Card temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }
}

void freeGame(Game* game) {
    // Free the cards array in the deck
    if (game->board->deck->cards != NULL) {
        free(game->board->deck->cards);
    }

    // Free the deck itself
    if (game->board->deck != NULL) {
        free(game->board->deck);
    }

    // Free the board
    if (game->board != NULL) {
        free(game->board);
    }

    // Free the players array
    if (game->players != NULL) {
        free(game->players);
    }
}




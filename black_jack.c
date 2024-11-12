#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_NAME_LEN 50
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

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


// Map values and suits to their string representations
const char* VALUE_NAMES[] = {"Ace", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King"};
const char* SUIT_NAMES[] = {"Hearts", "Diamonds", "Spades", "Clubs"};
const char* COLOR_NAMES[] = {"Red", "Black"};


void initializePlayer(Player* player);
void initializeBoard(Board* board);
void initializeDeck(Deck* deck);
void shuffleDeck(Deck* deck);
void freeGame(Game* game);

void dealCards(Game* game);
void RemoveFromDeck(Game* game,Card *card);
void InsertToDeck(Game* game,Card *card);

void printCards(Card* card);


int main() {
    // Create and initialize a red card (Ace of Hearts)
    Card redCard = {ACE, HEARTS, RED};

    // Create and initialize a black card (King of Spades)
    Card blackCard = {KING, SPADES, BLACK};

    // Test the printCards function with both cards
    printf("Red Card:\n");
    printCards(&redCard);

    printf("\nBlack Card:\n");
    printCards(&blackCard);

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

void dealCards(Game* game){
    shuffleDeck(game->board->deck);

    // Deal two cards to the first player
    game->players[0].card[0] = game->board->deck->cards[0];
    RemoveFromDeck(game, &game->board->deck->cards[0]);
    game->players[0].card[1] = game->board->deck->cards[0];
    RemoveFromDeck(game, &game->board->deck->cards[0]);

    // Deal two cards to the dealer
    game->board->dealerCards[0] = game->board->deck->cards[0];
    RemoveFromDeck(game, &game->board->deck->cards[0]);
    game->board->dealerCards[1] = game->board->deck->cards[0];
    RemoveFromDeck(game, &game->board->deck->cards[0]);
}

void RemoveFromDeck(Game* game, Card* card){
    Deck* deck = game->board->deck;
    int deckSize = 52;  // Assuming full deck initially; adjust as necessary if deck size changes

    int cardIndex = -1;
    // Find the card index in the deck
    for (int i = 0; i < deckSize; i++) {
        if (deck->cards[i].Suit == card->Suit && deck->cards[i].Value == card->Value) {
            cardIndex = i;
            break;
        }
    }

    // If card was found, shift remaining cards left to fill the gap
    if (cardIndex != -1) {
        for (int i = cardIndex; i < deckSize - 1; i++) {
            deck->cards[i] = deck->cards[i + 1];
        }
        deckSize--;  // Reduce the logical size of the deck
    }
}

void InsertToDeck(Game* game, Card* card){
    Deck* deck = game->board->deck;
    int deckSize = 52;  // Adjust based on how many cards are in the deck

    // Insert card at the end of the current deck size
    deck->cards[deckSize] = *card;
    deckSize++;  // Increase the logical size of the deck
}

void printCards(Card* card){
    // Check if the card is red or black
    if (card->Color == RED) {
        printf(ANSI_COLOR_RED);  // Set text color to red for red cards
    }

    // Print card information in a box with regular characters
    printf("+---------------+\n");
    printf("| %-13s |\n", VALUE_NAMES[card->Value]);
    printf("| %-13s |\n", SUIT_NAMES[card->Suit]);
    printf("| Color: %-6s |\n", COLOR_NAMES[card->Color]);
    printf("+---------------+\n");

    // Reset the color for subsequent text
    printf(ANSI_COLOR_RESET);
}



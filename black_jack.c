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
    Card card[52];
} Deck;

typedef struct
{
    Deck deck;
    Card dealerCards[2];
    double sumBetting;
} Board;

typedef struct
{
    Board board;
    Player players[2];
} Game;

void initializePlayer(Player* player);
void initializeBoard(Board* board);
void initializeDeck(Deck* deck);
void shuffleDeck(Deck* deck);

int main()
{
    return 0;
}

void initializePlayer(Player* player) {
    player->ChipSum = 250.0;
}

void initializeDeck(Deck* deck) {
    SUIT suits[4] = {HEARTS, DIAMONDS, SPADES, CLUBS};
    VALUE values[13] = {ACE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING};

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 13; j++) {
            int index = i * 13 + j;
            deck->card[index].Suit = suits[i];
            deck->card[index].Value = values[j];

            // Assign color based on suit
            if (deck->card[index].Suit == HEARTS || deck->card[index].Suit == DIAMONDS) {
                deck->card[index].Color = RED;
            } else {
                deck->card[index].Color = BLACK;
            }
        }
    }
}

void initializeBoard(Board* board) {
    initializeDeck(&board->deck);
}

void shuffleDeck(Deck* deck) {
    // Seed the random number generator
    srand((unsigned) time(NULL));

    for (int i = 51; i > 0; i--) {
        // Generate a random index between 0 and i
        int j = rand() % (i + 1);

        // Swap deck->card[i] with deck->card[j]
        Card temp = deck->card[i];
        deck->card[i] = deck->card[j];
        deck->card[j] = temp;
    }
}

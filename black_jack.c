#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_NAME_LEN 50
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define MAX_PLAYERS 4


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
    bool isLost;
    int bet;
} Player;

typedef struct
{
    Card* cards;      // Now a pointer to a dynamically allocated array of Cards
    int deckSize;
} Deck;

typedef struct
{
    Deck* deck;
    Card dealerCards[2];
    Player dealer;
    double sumBetting;
} Board;

typedef struct
{
    Player* players;  // Pointer to a dynamically allocated array of Players
    Board* board;
    int numPlayers;
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
void DetermineWinner(Game* game);
int CalculateScore(Card* card,int cardCount);
//newest functions
void placeBet(Player* player, double betAmmount); //This function allows a player to place a bet. It checks that the player has enough balance to place the bet.
void acceptBets(Game* game); //This function should iterate over all players and ask them to place their bets, storing the bet amounts for each player.
void resolveBets(Game *game); //After determining the winner, this function resolves the bets, awarding winnings to the player(s) who beat the dealer.

//need to make you
void dealerTurn(Game *game); //This function would handle the dealer's behavior, where the dealer reveals their second card and follows the rules to hit or stand.
void playerTurn(Player* player,Game* game); //same like dealer turn, if allow the player to choose weater hit,stand . maybe add:surrender
void startRound(Game* game); //main game loop


int main() {

    Deck deck;
    initializeDeck(&deck);
    shuffleDeck(&deck);

    Card card1 = deck.cards[0];
    Card card2 = deck.cards[1];

    // Test the printCards function with both cards

    printCards(&card1);


    printCards(&card2);

    return 0;
}

void initializePlayer(Player* player) {
    player->ChipSum = 250.0;
    player->isLost = false;
    player->bet = 0;
    strcpy(player->name, "Default Name");  // Optional: set a default name
}

void initializeDeck(Deck* deck) {
    deck->cards = malloc(52 * sizeof(Card));  // Allocate space for 52 cards
    deck->deckSize = 52;

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
    game->numPlayers = playerCount;
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

void RemoveFromDeck(Game* game, Card* card) {
    Deck* deck = game->board->deck;
    int cardIndex = -1;

    // Find the card index in the deck
    for (int i = 0; i < deck->deckSize; i++) {
        if (deck->cards[i].Suit == card->Suit && deck->cards[i].Value == card->Value) {
            cardIndex = i;
            break;
        }
    }

    // If card was found, shift remaining cards left to fill the gap
    if (cardIndex != -1) {
        for (int i = cardIndex; i < deck->deckSize - 1; i++) {
            deck->cards[i] = deck->cards[i + 1];
        }
        deck->deckSize--;  // Reduce the logical size of the deck
    }
}

void InsertToDeck(Game* game, Card* card) {
    Deck* deck = game->board->deck;

    // Insert card at the end of the current deck size
    deck->cards[deck->deckSize] = *card;
    deck->deckSize++;  // Increase the logical size of the deck
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

int calculateScore(Card* cards, int cardCount) {
    int score = 0;
    int aces = 0;  // Track number of Aces to adjust value as 1 or 11

    for (int i = 0; i < cardCount; i++) {
        if (cards[i].Value >= TWO && cards[i].Value <= TEN) {
            score += cards[i].Value + 1;  // Add face value of cards (TWO is 1, etc.)
        } else if (cards[i].Value >= JACK && cards[i].Value <= KING) {
            score += 10;  // Face cards are worth 10 points
        } else if (cards[i].Value == ACE) {
            score += 11;  // Initially, count Ace as 11
            aces++;
        }
    }

    // Adjust score for Aces if score > 21
    while (score > 21 && aces > 0) {
        score -= 10;  // Reduce 11 to 1 for an Ace
        aces--;
    }

    return score;
}

void DetermineWinner(Game* game) {
    int player1Score = calculateScore(game->players[0].card, 2);
    int player2Score = calculateScore(game->players[1].card, 2);
    int dealerScore = calculateScore(game->board->dealerCards, 2);

    // Output the scores
    printf("Player 1 Score: %d\n", player1Score);
    printf("Player 2 Score: %d\n", player2Score);
    printf("Dealer Score: %d\n", dealerScore);

    // Determine if each player or dealer busts
    bool player1Bust = (player1Score > 21);
    bool player2Bust = (player2Score > 21);
    bool dealerBust = (dealerScore > 21);

    // Determine the winner for each player individually
    if (player1Bust) {
        printf("Player 1 busts! ");
    } else if (dealerBust || player1Score > dealerScore) {
        printf("Player 1 wins against Dealer!\n");
    } else if (player1Score == dealerScore) {
        printf("Player 1 ties with Dealer!\n");
    } else {
        printf("Dealer wins against Player 1!\n");
    }

    if (player2Bust) {
        printf("Player 2 busts! ");
    } else if (dealerBust || player2Score > dealerScore) {
        printf("Player 2 wins against Dealer!\n");
    } else if (player2Score == dealerScore) {
        printf("Player 2 ties with Dealer!\n");
    } else {
        printf("Dealer wins against Player 2!\n");
    }

    // Update the status of each player if they lost
    game->players[0].isLost = player1Bust || (!dealerBust && dealerScore > player1Score);
    game->players[1].isLost = player2Bust || (!dealerBust && dealerScore > player2Score);
}

void placeBet(Player* player,double betAmount){

    if(betAmount > player->ChipSum )
    {
        printf("bet is higher than you chip amount, lower the bet\n");
    }
    else
    {
        player->bet = betAmount;
        player->ChipSum -= betAmount;
        printf("Bet Placed: %d\n", betAmount);
    }
}

void acceptBets(Game* game) {
    for (int i = 0; i < game->numPlayers; i++) {
        int betAmount;
        printf("Player %d, enter your bet: ", i+1);
        scanf("%d", &betAmount);
        placeBet(&game->players[i], betAmount);
    }
}

void resolveBets(Game* game){

    DetermineWinner(game);

    for(int i=0; i<game->numPlayers; i++)
    {
        if(!game->players[i].isLost)
        {
            game->players[i].ChipSum += 2 * game->players[i].bet;
            printf("Player %d Wins Amount Of: %d",i+1, game->players[i].bet * 2);
        }
        else
        {
            printf("Player %d loses their bet of %d.\n", i+1, game->players[i].bet);
        }
    }
}



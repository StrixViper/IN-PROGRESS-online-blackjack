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
    bool isTie;
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
void printCard(Card* card);
void DetermineWinner(Game* game);
int CalculateScore(Card* card,int cardCount);
void placeBet(Player* player, double betAmmount); //This function allows a player to place a bet. It checks that the player has enough balance to place the bet.
void acceptBets(Game* game); //This function should iterate over all players and ask them to place their bets, storing the bet amounts for each player.
void resolveBets(Game *game); //After determining the winner, this function resolves the bets, awarding winnings to the player(s) who beat the dealer.
void dealerTurn(Game *game); //This function would handle the dealer's behavior, where the dealer reveals their second card and follows the rules to hit or stand.
void playerTurn(Player* player,Game* game); //same like dealer turn, if allow the player to choose weater hit,stand . maybe add:surrender
void startRound(Game* game); //main game loop
void handleBetting(Game* game);
void getPlayersDetails(Game* game);
int getPlayersCount();
void ClearConsole();

int main() {

    Game game;
    int count = getPlayersCount();
    initializeGame(&game,count);

    getPlayersDetails(&game);

    startGame(&game);

    return 0;
}

void initializePlayer(Player* player) {
    player->ChipSum = 250.0;
    player->isLost = false;
    player->isTie = false;
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

void printCard(Card* card){
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
    int dealerScore = calculateScore(game->board->dealerCards, 2);  // Assuming 2 dealer cards

    // Output the dealer's score
    printf("Dealer Score: %d\n", dealerScore);

    bool dealerBust = (dealerScore > 21);

    // Loop through each player to calculate their scores and determine the result
    for (int i = 0; i < game->numPlayers; i++) {
        int playerScore = calculateScore(game->players[i].card, 2);  // Assuming 2 cards per player

        // Output the player's score
        printf("%s Score: %d\n", game->players[i].name, playerScore);

        bool playerBust = (playerScore > 21);

        // Determine the result for the player against the dealer
        if (playerBust) {
            printf("%s busts!\n", game->players[i].name);
        } else if (dealerBust || playerScore > dealerScore) {
            printf("%s wins against Dealer!\n", game->players[i].name);
        } else if (playerScore == dealerScore) {
            printf("%s ties with Dealer!\n", game->players[i].name);
            game->players[i].isTie = true;
        } else {
            printf("Dealer wins against %s!\n", game->players[i].name);
        }

        // Update the player's lost status
        game->players[i].isLost = playerBust || (!dealerBust && dealerScore > playerScore);
    }
}

void placeBet(Player* player, double betAmount) {
    if (betAmount > player->ChipSum) {
        printf("Bet is higher than your chip amount, lower the bet\n");
    } else {
        player->bet = betAmount;
        player->ChipSum -= betAmount;
        printf("Bet Placed: %.2f\n", betAmount);  // Corrected format specifier for double
    }
}

void acceptBets(Game* game) {
    for (int i = 0; i < game->numPlayers; i++) {
        double betAmount;  // Use double for betAmount to match the function signature
        printf("Player %d, enter your bet: ", i + 1);
        scanf("%lf", &betAmount);  // Use %lf to read a double
        placeBet(&game->players[i], betAmount);
    }
}

void resolveBets(Game* game){

    DetermineWinner(game);

    for(int i=0; i<game->numPlayers; i++)
    {
        if(!game->players[i].isLost)
        {
            if(game->players[i].isTie)
            {
            game->players[i].ChipSum += game->players[i].bet;
            printf("Player %d Tie And Split Amount Of: %d",i+1, game->players[i].bet);
            }
            else
            {
            game->players[i].ChipSum += 2 * game->players[i].bet;
            printf("Player %d Wins Amount Of: %d ",i+1, game->players[i].bet * 2);

            }
        }
        else
        {
            printf("Player %d loses their bet of %d.\n", i+1, game->players[i].bet);
        }
    }
}

void playerTurn(Player* player, Game* game) {
    int playerScore = calculateScore(player->card, 2);
    int cardCount = 2;
    char choice;

    printf("%s's turn:\n", player->name);
    printf("Initial hand:\n");
    for (int i = 0; i < cardCount; i++) {
        printCard(&player->card[i]);
    }
    printf("%s's initial score: %d\n", player->name, playerScore);

    // Player decides to hit, stand, or surrender
    while (playerScore < 21) {
        printf("Choose an action: (h)it, (s)tand, or (r)surrender: ");
        scanf(" %c", &choice);

        if (choice == 'h') {  // Hit
            printf("%s hits.\n", player->name);
            player->card[cardCount] = game->board->deck->cards[0];
            RemoveFromDeck(game, &game->board->deck->cards[0]);
            printCard(&player->card[cardCount]);
            cardCount++;

            playerScore = calculateScore(player->card, cardCount);
            printf("%s's new score: %d\n", player->name, playerScore);

        } else if (choice == 's') {  // Stand
            printf("%s stands with a score of %d.\n", player->name, playerScore);
            break;

        } else if (choice == 'r') {  // Surrender
            printf("%s surrenders.\n", player->name);
            player->isLost = true;
            player->ChipSum -= player->bet / 2;  // Lose half of the bet
            break;

        } else {
            printf("Invalid choice. Please choose again.\n");
        }
    }

    if (playerScore > 21) {
        printf("%s busts with a score of %d!\n", player->name, playerScore);
        player->isLost = true;
    }
}

void dealerTurn(Game *game) {
    int dealerScore = calculateScore(game->board->dealerCards, 2);
    int cardCount = 2;

    // Dealer reveals their hidden card
    printf("Dealer's cards:\n");
    for (int i = 0; i < cardCount; i++) {
        printCard(&game->board->dealerCards[i]);
    }
    printf("Dealer's initial score: %d\n", dealerScore);

    // Dealer hits until reaching at least 17
    while (dealerScore < 17) {
        printf("Dealer hits.\n");

        // Draw a new card
        game->board->dealerCards[cardCount] = game->board->deck->cards[0];
        RemoveFromDeck(game, &game->board->deck->cards[0]);

        printCard(&game->board->dealerCards[cardCount]);
        cardCount++;

        // Recalculate dealer's score with new card
        dealerScore = calculateScore(game->board->dealerCards, cardCount);
        printf("Dealer's new score: %d\n", dealerScore);
    }

    // Dealer stands if score is 17 or higher
    if (dealerScore >= 17) {
        printf("Dealer stands with a score of %d.\n", dealerScore);
    }
}

void startGame(Game* game) {
    bool gameOver = false;

    while (!gameOver) {
        // 1. Accept player bets
        printf("Starting a new round!\n");
        acceptBets(game);

        // 2. Deal initial cards to players and dealer
        dealCards(game);

        // 3. Player turns
        for (int i = 0; i < game->numPlayers; i++) {
            if (!game->players[i].isLost) {
                printf("Player %s's turn:\n", game->players[i].name);
                playerTurn(&game->players[i], game);
            }
        }

        // 4. Dealer turn
        printf("Dealer's turn:\n");
        dealerTurn(game);

        // 5. Determine the winner(s)
        DetermineWinner(game);

        // 6. Resolve bets
        resolveBets(game);

        // 7. Check if players want to continue or end the game
        printf("Do you want to play another round? (y/n): ");
        char choice;
        scanf(" %c", &choice);
        if (choice == 'n' || choice == 'N') {
            gameOver = true;
        } else {
            // Reset player states and game board for the next round
            for (int i = 0; i < game->numPlayers; i++) {
                game->players[i].bet = 0;
                game->players[i].isLost = false;
            }
            initializeDeck(game->board->deck);
            shuffleDeck(game->board->deck);
        }
    }

    printf("Game Over! Thanks for playing.\n");
}

void handleBetting(Game *game) {
    for (int i = 0; i < game->numPlayers; i++) {
        int betAmount;
        printf("%s, enter your bet amount (Balance: %d): ", game->players[i].name, game->players[i].ChipSum);
        scanf("%d", &betAmount);

        // Validate bet
        while (betAmount <= 0 || betAmount > game->players[i].ChipSum) {
            if (betAmount <= 0) {
                printf("Bet must be a positive amount. Try again: ");
            } else {
                printf("Bet exceeds available balance. Try again: ");
            }
            scanf("%d", &betAmount);
        }

        // Place the bet and update balance
        placeBet(&game->players[i], betAmount);
        printf("%s placed a bet of %d. Remaining balance: %d\n", game->players[i].name, betAmount, game->players[i].ChipSum);
    }
}

int getPlayersCount(){
    int playerAmount;

    printf("insert the number of players: ");
    scanf("%d",&playerAmount);

    return playerAmount;

}

void getPlayersDetails(Game *game) {
    for (int i = 0; i < game->numPlayers; i++) {
        printf("Player %d, please enter your name: ", i + 1);

        // Limit input to MAX_NAME_LEN-1 to leave space for null terminator
        if (scanf("%49s", game->players[i].name) != 1) {
            printf("Error reading name for player %d\n", i + 1);
            game->players[i].name[0] = '\0';  // Set an empty name in case of error
        }

        // Clear remaining input buffer in case the name was too long
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
    }
}

void ClearConsole() {
    #if defined(_WIN32)
        system("cls"); // For Windows
    #else
        system("clear"); // For Linux and macOS
    #endif
}



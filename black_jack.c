#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_NAME_LEN 50
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define MAX_PLAYERS 4
#define MAX_CARDS 50

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
    Card card[MAX_CARDS];
    char name[MAX_NAME_LEN];
    double ChipSum;
    bool isLost;
    int bet;
    bool isTie;
    int countCard;
} Player;

typedef struct
{
    Card* cards;      // Now a pointer to a dynamically allocated array of Cards
    int deckSize;
} Deck;

typedef struct
{
    Deck* deck;
    Card dealerCards[3];
    int dealCardCount;
    Player dealer;
    double sumBetting;
} Board;

typedef struct
{
    Player* players;  // Pointer to a dynamically allocated array of Players
    Board* board;
    int numPlayers;
} Game;

//##########----- STRUCTS FOR THE HISTORY MOVES -----################



//####################################################################


// Map values and suits to their string representations
const char* VALUE_NAMES[] = {"Ace", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King"};
const char* SUIT_NAMES[] = {"Hearts", "Diamonds", "Spades", "Clubs"};
const char* COLOR_NAMES[] = {"Red", "Black"};


void initializePlayer(Player* player); // This function initializes a player with default values (e.g., name, chips, and other necessary attributes).

void initializeBoard(Board* board);// This function initializes the game board, including setting up the dealer's cards and any other board-related data.

void initializeDeck(Deck* deck); // This function initializes the deck, ensuring that all cards are available for use in the game.

void shuffleDeck(Deck* deck); // This function shuffles the deck to ensure randomness before cards are dealt.

void freeGame(Game* game); // This function cleans up the game resources (e.g., freeing allocated memory for players, deck, etc.) when the game ends.

void dealCards(Game* game); // This function deals cards to all players and the dealer at the start of a round.

void RemoveFromDeck(Game* game, Card* card); // This function removes a specific card from the deck after it has been dealt to a player or dealer.

void InsertToDeck(Game* game, Card* card); // This function inserts a card back into the deck (useful when reshuffling or returning cards to the deck).

void printCard(Card* card); // This function prints out the details of a single card (e.g., the card's rank and suit).

void DetermineWinner(Game* game); // This function determines the winner of the round by comparing the scores of all players and the dealer. It will announce the result accordingly.

int CalculateScore(Card* card, int cardCount); // This function calculates the score of a hand, based on the cards in the hand. It sums up the values of the cards.

void placeBet(Player* player, double betAmount); // This function allows a player to place a bet. It checks that the player has enough balance to place the bet.

void acceptBets(Game* game); // This function iterates over all players and asks them to place their bets, storing the bet amounts for each player.

void resolveBets(Game* game); // After determining the winner, this function resolves the bets, awarding winnings to the player(s) who beat the dealer.

void dealerTurn(Game* game); // This function handles the dealer's behavior, where the dealer reveals their second card and follows the rules to hit or stand.

void playerTurn(Player* player, Game* game); // This function handles the player's turn, allowing them to choose whether to hit, stand, or even surrender.

void startRound(Game* game); // This is the main game loop for a round, coordinating the sequence of actions from dealing cards to determining the winner.

void handleBetting(Game* game); // This function manages the betting phase, allowing each player to place their bets before the cards are dealt.

void getPlayersDetails(Game* game); // This function asks players for their details (e.g., name), which are then stored in the game structure.

int getPlayersCount(); // This function asks the user how many players are participating in the game and returns the number of players.

void ClearConsole(); // This function clears the console screen, often used to refresh the display during gameplay.

void PrintBalance(Player* player); // This function print the chip sum of a player

void printRules(); // This function print the rules of the game

void displayMenu(); //This function display the menu for the game

int main() {
    displayMenu();
    return 0;
}


void PrintBalance(Player* player) {
    printf("%s Balance: %.2f\n", player->name, player->ChipSum); // Add 'player->name' for the name
}

void initializePlayer(Player* player) {
    player->ChipSum = 250.0;
    player->isLost = false;
    player->isTie = false;
    player->bet = 0;
    player->countCard = 2;
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
    board->dealCardCount = 2;
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

void dealCards(Game* game) {
    // Shuffle the deck before dealing
    shuffleDeck(game->board->deck);

    // Deal two cards to each player
    for (int i = 0; i < game->numPlayers; i++) {
        for (int j = 0; j < 2; j++) {
            game->players[i].card[j] = game->board->deck->cards[0];
            RemoveFromDeck(game, &game->board->deck->cards[0]);
        }
    }

    // Deal two cards to the dealer
    for (int j = 0; j < 2; j++) {
        game->board->dealerCards[j] = game->board->deck->cards[0];
        RemoveFromDeck(game, &game->board->deck->cards[0]);
    }
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

    int dealerScore = calculateScore(game->board->dealerCards, game->board->dealCardCount);  // Using dealer's actual card count
    printf("Dealer Score: %d\n", dealerScore);

    bool dealerBust = (dealerScore > 21);

    if(dealerScore > 21)
        {
            printf("Dealer Busts!!! \n");
        }

    // Loop through each player to calculate their scores and determine the result
    for (int i = 0; i < game->numPlayers; i++) {

        // Use the actual card count for each player
        int playerScore = calculateScore(game->players[i].card, game->players[i].countCard);

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
    player->bet = betAmount;
    player->ChipSum -= betAmount;
}

void acceptBets(Game* game) {
    for (int i = 0; i < game->numPlayers; i++) {
        double betAmount;

        printf("%s Balance: %.2f\n",game->players[i].name,game->players[i].ChipSum);

        // Continuously ask for a valid bet
        printf("Player %d, enter your bet: ", i + 1);
        scanf("%lf", &betAmount);

        while (betAmount > game->players[i].ChipSum || betAmount <= 0) {
            if (betAmount > game->players[i].ChipSum) {
                printf("Bet is higher than your chip amount, lower the bet.\n");
            } else if (betAmount <= 0) {
                printf("Bet must be greater than zero.\n");
            }
            printf("Player %d, enter your bet: ", i + 1);
            scanf("%lf", &betAmount);
        }

        placeBet(&game->players[i], betAmount);
        printf("Bet Placed: %.2f\n", betAmount);
        printf("Balance Left After The Bet: %.2f \n",game->players[i].ChipSum);
    }
}

void resolveBets(Game* game){


    for(int i=0; i<game->numPlayers; i++)
    {
        if(!game->players[i].isLost)
        {
            if(game->players[i].isTie)
            {
            game->players[i].ChipSum += game->players[i].bet;
            printf("Player %s Tie And Split Amount Of: %d \n",game->players[i].name, game->players[i].bet);

            }
            else
            {
            game->players[i].ChipSum += 2 * game->players[i].bet;
            printf("Player %s Wins Amount Of: %d \n",game->players[i].name, game->players[i].bet * 2);

            }
        }
        else
        {
            printf("Player %s loses their bet of %d.\n", game->players[i].name, game->players[i].bet);
        }
        PrintBalance(&game->players[i]);
    }
}

void playerTurn(Player* player, Game* game) {
    // Initialize player score with the initial card count (assumed to be 2)

    int playerScore = calculateScore(player->card, player->countCard);
    char choice;

    printf("%s's turn:\n", player->name);
    printf("Initial hand:\n");
    for (int i = 0; i < player->countCard; i++) {
        printCard(&player->card[i]);
    }
    printf("%s's initial score: %d\n", player->name, playerScore);

    // Player decides to hit, stand, or surrender
    while (playerScore < 21) {
        printf("Choose an action: (h)it, (s)tand, or (r)surrender: ");
        scanf(" %c", &choice);

        if (choice == 'h') {  // Hit
            printf("%s hits.\n", player->name);
            player->card[player->countCard] = game->board->deck->cards[0];  // Add a new card
            RemoveFromDeck(game, &game->board->deck->cards[0]);             // Remove that card from the deck
            printCard(&player->card[player->countCard]);
            player->countCard++;  // Increment countCard to reflect new card
            playerScore = calculateScore(player->card, player->countCard);  // Update player score with new card count
            printf("%s's new score: %d\n", player->name, playerScore);

            // Debugging output
            printf("player card count: %d\n", player->countCard);

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
        game->board->dealCardCount = 3;
        // Draw a new card
        game->board->dealerCards[cardCount] = game->board->deck->cards[0];
        RemoveFromDeck(game, &game->board->deck->cards[0]);

        printCard(&game->board->dealerCards[cardCount]);
        cardCount++;

        // Recalculate dealer's score with new card
        dealerScore = calculateScore(game->board->dealerCards, cardCount);
        if(dealerScore<=21)
            {
                printf("Dealer's new score: %d\n", dealerScore);
            }
    }

    // Dealer stands if score is 17 or higher
    if (dealerScore >= 17) {
        int dealerScoreAfter = calculateScore(game->board->dealerCards, cardCount);
            if(dealerScore<=21)
                {
                    printf("Dealer stands with a score of %d.\n", dealerScore);
                }
    }
}

void startGame(Game* game) {
    bool gameOver = false;

    while (!gameOver) {
        ClearConsole();
        // 1. Accept player bets
        printf("Starting a new round!\n");
        acceptBets(game);

        // 2. Deal initial cards to players and dealer
        dealCards(game);

        // 3. Player turns
        for (int i = 0; i < game->numPlayers; i++) {
            if (!game->players[i].isLost) {
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

void printRules() {
    printf("\n****** BLACKJACK GAME RULES ******\n");
    printf("1. The game is played with one or more decks of 52 cards.\n");
    printf("2. The goal is to get as close to 21 points as possible, without exceeding 21.\n");
    printf("3. Number cards (2-10) are worth their face value.\n");
    printf("4. Face cards (Jack, Queen, King) are each worth 10 points.\n");
    printf("5. Aces can be worth 1 or 11 points, whichever is more beneficial.\n");
    printf("6. Players are dealt two cards, and the dealer is dealt one card face up.\n");
    printf("7. Players can 'Hit' to take another card or 'Stand' to hold their total.\n");
    printf("8. Players who exceed 21 points lose automatically (bust).\n");
    printf("9. If the player's total is higher than the dealer's without busting, they win.\n");
    printf("10. If the dealer has a higher total, the dealer wins.\n");
    printf("*************************************\n\n");
}

void displayMenu() {
    int choice;
    int count;
    Game game;

    while (1) {
        // Stylish menu display
        printf("\n\n********** BLACKJACK GAME **********\n");
        printf("* 1. Start a New Game             *\n");
        printf("* 2. View Game Rules              *\n");
        printf("************************************\n");
        printf("Please choose an option (1 or 2): ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                ClearConsole();
                count = getPlayersCount();
                initializeGame(&game,count);
                getPlayersDetails(&game);
                startGame(&game);
               break;
            case 2:
                ClearConsole();
                printRules();
                break;
            default:
                printf("\nInvalid choice. Please try again.\n");
        }
    }
}



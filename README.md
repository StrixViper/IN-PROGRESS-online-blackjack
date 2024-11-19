# Blackjack Game in C

Welcome to the Blackjack game! This is a simple terminal-based implementation of the classic card game Blackjack, written in C. The game features an interactive menu where users can either start a new game or view the game rules.

## Features
- Start a new game of Blackjack.
- View the game rules at any time.
- Simple terminal interface.

## Getting Started

To get started with this Blackjack game, you'll need a C compiler (like GCC) installed on your system.

### Prerequisites

Make sure you have the following installed:
- A C compiler (e.g., GCC)
- A terminal/command prompt for compiling and running the program

### Installation

1. Clone this repository:
   ```bash
   git clone https://github.com/StrixViper/IN-PROGRESS-online-blackjack
   ```

2. Navigate to the project directory:
   ```bash
   cd IN-PROGRESS-online-blackjack
   ```

3. Compile the C source code:
   ```bash
   gcc -o IN-PROGRESS-online-blackjack black_jack.c
   ```

4. Run the game:
   ```bash
   ./IN-PROGRESS-online-blackjack
   ```

## How to Play

Upon running the game, you'll be presented with the following menu:

```
********** BLACKJACK GAME **********
* 1. Start a New Game             *
* 2. View Game Rules              *
************************************
```

- **Option 1**: Start a new game of Blackjack.
- **Option 2**: View the rules of the game.

### Game Rules

1. The game is played with one or more decks of 52 cards.
2. The goal is to get as close to 21 points as possible, without exceeding 21.
3. Number cards (2-10) are worth their face value.
4. Face cards (Jack, Queen, King) are each worth 10 points.
5. Aces can be worth 1 or 11 points, whichever is more beneficial.
6. Players are dealt two cards, and the dealer is dealt one card face up.
7. Players can 'Hit' to take another card or 'Stand' to hold their total.
8. Players who exceed 21 points lose automatically (bust).
9. If the player's total is higher than the dealer's without busting, they win.
10. If the dealer has a higher total, the dealer wins.

For more detailed instructions on playing, check out the **Game Rules** section within the menu.

## Contributing

If you'd like to contribute to this project, feel free to fork the repository, make changes, and submit a pull request. Contributions are welcome!

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

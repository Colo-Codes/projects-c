/*************************
 * By Damian Demasi
 * Date: 17/04/2021
 * Requirements:
 *  Write a program that plays Tic Tac Toe. The game is played on a 3x3 grid, by 2 players who take turns.
 *  Use three functions:
 *      - Check to see if a player won or the game is draw.
 *      - Redraw the board for each player turn.
 *      - Set a char array with a selection and check for an invalid selection.
 *************************/

#include <stdio.h>

// Function declarations
void drawBoard();

// Function definitions
int main()
{
    drawBoard();

    return 0;
}

void drawBoard()
{
    printf("\n\t  _____ _        _____            _____           ");
    printf("\n\t |_   _(_) ___  |_   _|_ _  ___  |_   _|__   ___  ");
    printf("\n\t   | | | |/ __|   | |/ _` |/ __|   | |/ _ \\ / _ \\ ");
    printf("\n\t   | | | | (__    | | (_| | (__    | | (_) |  __/ ");
    printf("\n\t   |_| |_|\\___|   |_|\\__,_|\\___|   |_|\\___/ \\___| ");
    printf("\n\n\t\tPlayer 1 is playing (X), Player 2 is playing (O)\n");
    printf("\n\t\t\t     |     |     ");
    printf("\n\t\t\t  1  |  2  |  3  ");
    printf("\n\t\t\t_____|_____|_____");
    printf("\n\t\t\t     |     |     ");
    printf("\n\t\t\t  4  |  5  |  6  ");
    printf("\n\t\t\t_____|_____|_____");
    printf("\n\t\t\t     |     |     ");
    printf("\n\t\t\t  7  |  8  |  9  ");
    printf("\n\t\t\t     |     |     ");
}
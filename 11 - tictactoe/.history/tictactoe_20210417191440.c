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
#include <stdlib.h> // For system()

#define MAX_ATTEMPTS 3

// Function declarations
void drawBoard(int boardArray[3][3]);
int checkValid(int selection, int boardArray[3][3]);
int checkWonDraw(int boardArray[3][3]);
void updateBoard(int selection, int activePlayer, int boardArray[3][3]);

// Function definitions
int main()
{
    int selection = 0,
        endGame = 0,
        wrongSelections = 1,
        activePlayer = 0;

    // Game initial state
    int boardArray[3][3] = {
        {'1', '2', '3'},
        {'4', '5', '6'},
        {'7', '8', '9'}
    };

    while(wrongSelections < MAX_ATTEMPTS + 1) // Allows 3 wrong guesses
    {

        if (endGame == 0)
        {
            // Find active player
            if (activePlayer == 0)
            {
                activePlayer = 1;
            }
            else if (activePlayer == 1)
            {
                activePlayer = 2;
            }
            else
            {
                activePlayer = 1;
            }
        }

        // Present the board and select player
        drawBoard(boardArray);

        // Calculate current turn
        if (endGame == 0)
        {
            // Get selection
            printf("\n\n\tPlayer %d - select a position: ", activePlayer);
            scanf("%d", &selection);
            
            // Check if selection is valid
            while (wrongSelections < MAX_ATTEMPTS + 1) // Allows 3 wrong guesses
            {
                if(checkValid(selection, boardArray) != 1 && selection != 0)
                {
                    printf("\n\n\tPlayer %d - Wrong selection, please try again (%d of %d attempts): ", activePlayer, wrongSelections, MAX_ATTEMPTS);
                    scanf("%d", &selection);
                    
                    wrongSelections++;

                    if (wrongSelections == 3)
                        printf("\n\n\t! ! ! GAME OVER - Too many wrong options selected. ! ! !\n\n");
                }
                else
                {
                    break;
                }
            }
        }
        else if (endGame == 1)
        {
            printf("\n\n\t~ ~ ~ ~ CONGRATULATIONS player %d!!! You won the game!!! ~ ~ ~ ~\n\n", activePlayer);
            break;
        }
        else
        {
            break;
        }
        
        // Update board
        updateBoard(selection, activePlayer, boardArray);

        // Check game status
        endGame = checkWonDraw(boardArray);

        // Check draw
        if (endGame == 2)
        {
            drawBoard(boardArray);
            
            printf("\n\n\t! ! ! GAME OVER - The game ended in draw. ! ! !\n\n");
            break;
        }
    }

    return 0;
}

void drawBoard(int boardArray[3][3])
{
    system("clear"); // Clear screen on *nix
    printf("\n\t  _____ _        _____            _____           ");
    printf("\n\t |_   _(_) ___  |_   _|_ _  ___  |_   _|__   ___  ");
    printf("\n\t   | | | |/ __|   | |/ _` |/ __|   | |/ _ \\ / _ \\ ");
    printf("\n\t   | | | | (__    | | (_| | (__    | | (_) |  __/ ");
    printf("\n\t   |_| |_|\\___|   |_|\\__,_|\\___|   |_|\\___/ \\___| ");
    printf("\n\n\tPlayer 1 is playing (X), Player 2 is playing (O)\n");
    printf("\n\t\t\t     |     |     ");
    printf("\n\t\t\t  %c  |  %c  |  %c  ", boardArray[0][0], boardArray[0][1], boardArray[0][2]);
    printf("\n\t\t\t_____|_____|_____");
    printf("\n\t\t\t     |     |     ");
    printf("\n\t\t\t  %c  |  %c  |  %c  ", boardArray[1][0], boardArray[1][1], boardArray[1][2]);
    printf("\n\t\t\t_____|_____|_____");
    printf("\n\t\t\t     |     |     ");
    printf("\n\t\t\t  %c  |  %c  |  %c  ", boardArray[2][0], boardArray[2][1], boardArray[2][2]);
    printf("\n\t\t\t     |     |     ");
}

int checkValid(int selection, int boardArray[3][3])
{
    int validFlag = 1;

    switch (selection)
    {
        case 0:
            validFlag = 1;
            break;
        case 1:
        case 2:
        case 3:
            if(boardArray[0][selection - 1] == 'X' || boardArray[0][selection - 1] == 'O')
                validFlag = 0;
            break;
        
        case 4:
        case 5:
        case 6:
            if(boardArray[1][selection - 4] == 'X' || boardArray[1][selection - 4] == 'O')
                validFlag = 0;
            break;
        
        case 7:
        case 8:
        case 9:
            if(boardArray[2][selection - 7] == 'X' || boardArray[2][selection - 7] == 'O')
                validFlag = 0;
            break;

        default:
            validFlag = 0;
            break;
    }

    return validFlag;
}

int checkWonDraw(int boardArray[3][3])
{
    int endGame = 0;
    int i, j;
    char k;

    // Check for winning combinations
    if ((boardArray[0][0] == boardArray[0][1] && boardArray[0][1] == boardArray[0][2]) ||
        (boardArray[1][0] == boardArray[1][1] && boardArray[1][1] == boardArray[1][2]) ||
        (boardArray[2][0] == boardArray[2][1] && boardArray[2][1] == boardArray[2][2]) ||

        (boardArray[0][0] == boardArray[1][0] && boardArray[1][0] == boardArray[2][0]) ||
        (boardArray[0][1] == boardArray[1][1] && boardArray[1][1] == boardArray[2][1]) ||
        (boardArray[0][2] == boardArray[1][2] && boardArray[1][2] == boardArray[2][2]) ||
        
        (boardArray[0][0] == boardArray[1][1] && boardArray[1][1] == boardArray[2][2]) ||
        (boardArray[2][0] == boardArray[1][1] && boardArray[1][1] == boardArray[0][2]))
    {
        // Game won
        endGame = 1;
    }
    else
    {
        // Check for no free positions
        if ((boardArray[0][0] != '1' && boardArray[0][1] != '2' && boardArray[0][2] != '3') &&
            (boardArray[1][0] != '4' && boardArray[1][1] != '5' && boardArray[1][2] != '6') &&
            (boardArray[2][0] != '7' && boardArray[2][1] != '8' && boardArray[2][2] != '9'))
        {
            // Game in draw
            endGame = 2;
        }
    }

    return endGame;
}

void updateBoard(int selection, int activePlayer, int boardArray[3][3])
{
    char drawSelection = '0';

    // Choose type of selection to draw according to active player
    (activePlayer == 1) ? drawSelection = 'X' : (drawSelection = 'O');
    
    if(selection != 0)
    {
        switch (selection)
        {
        case 1:
            boardArray[0][0] = drawSelection;
            break;
        
        case 2:
            boardArray[0][1] = drawSelection;
            break;
        
        case 3:
            boardArray[0][2] = drawSelection;
            break;
        
        case 4:
            boardArray[1][0] = drawSelection;
            break;
        
        case 5:
            boardArray[1][1] = drawSelection;
            break;
        
        case 6:
            boardArray[1][2] = drawSelection;
            break;
        
        case 7:
            boardArray[2][0] = drawSelection;
            break;
        
        case 8:
            boardArray[2][1] = drawSelection;
            break;
        
        case 9:
            boardArray[2][2] = drawSelection;
            break;
        
        default:
            break;
        }
    }
}
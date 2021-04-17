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

// Global variables
int boardArray[3][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'}
};
int activePlayer = 0;

// Function declarations
void drawBoard(int selection);
int checkValid(int selection);
int checkWonDraw();

// Function definitions
int main()
{
    int selection = 0, endGame = 0, turns = 0;
    
    while(turns < 10)
    {
        endGame = checkWonDraw();
        
        if (endGame == 1)
        {
            printf("\n\n\tCONGRATULATIONS player %d!!! You won the game!!!", activePlayer);
            break;
        }
        else if (endGame == 2)
        {
            printf("\n\n\tGAME OVER! The game ended in draw...");
            break;
        }
        else
        {
            // Check if selection is valid
            if(checkValid(selection) || selection == 0)
            {
                drawBoard(selection);
            }
            scanf("%d", &selection);

            // Debug
            printf("\n\n\nselection: %d", selection);
        }
        turns++;
    }

    return 0;
}

void drawBoard(int selection)
{
    char drawSelection = '0';

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
    
    // Choose type of selection to draw according to active player
    (activePlayer == 1) ? drawSelection = 'O' : (drawSelection = 'X');
    
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

    system("clear");// Clear screen on *nix
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
    printf("\n\n\tPlayer %d - select a position: ", activePlayer);

    // return selection;
}

int checkValid(int selection)
{
    int validFlag = 1;

    switch (selection)
    {
        case 1:
        case 2:
        case 3:
            if(boardArray[0][selection - 1] == 'X' || boardArray[0][selection - 1] == 'O')
                validFlag = 0;
            break;
        
        case 4:
        case 5:
        case 6:
            if(boardArray[1][selection - 1] == 'X' || boardArray[1][selection - 1] == 'O')
                validFlag = 0;
            break;
        
        case 7:
        case 8:
        case 9:
            if(boardArray[2][selection - 1] == 'X' || boardArray[2][selection - 1] == 'O')
                validFlag = 0;
            break;

        default:
            validFlag = 0;
            break;
    }

    return validFlag;
}

int checkWonDraw()
{
    int endGame = 0;
    int i, j, k;

    if((boardArray[0][0] == boardArray[0][1] == boardArray[0][2]) ||
        (boardArray[1][0] == boardArray[1][1] == boardArray[1][2]) ||
        (boardArray[2][0] == boardArray[2][1] == boardArray[2][2]) ||

        (boardArray[0][0] == boardArray[1][0] == boardArray[2][0]) ||
        (boardArray[0][1] == boardArray[1][1] == boardArray[2][1]) ||
        (boardArray[0][2] == boardArray[1][2] == boardArray[2][2]) ||
        
        (boardArray[0][0] == boardArray[1][1] == boardArray[2][2]) ||
        (boardArray[2][0] == boardArray[1][1] == boardArray[0][2]))
    {
        // Game won
        endGame = 1;
    }
    else
    {
        int countDraw = 0;

        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < 3; j++)
            {
                for (k = 1; k < 10; k++)
                {
                    if (boardArray[i][j] != k)
                        countDraw++;
                }
            }
        }
        if (countDraw == 9)
        {
            // Game in draw
            endGame = 2;
        }
    }

    return endGame;
}
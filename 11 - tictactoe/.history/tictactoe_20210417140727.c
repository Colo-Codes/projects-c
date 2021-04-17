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

// Global variables
int boardArray[3][3] = {0};
int activePlayer = 0;

// Function declarations
int drawBoard();

// Function definitions
int main()
{
    int selection = 0;
    
    while()
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

    return 0;
}

void drawBoard(int selection)
{
    char drawSelection = '0';
    char opt1 = '1';
    char opt2 = '2';
    char opt3 = '3';
    char opt4 = '4';
    char opt5 = '5';
    char opt6 = '6';
    char opt7 = '7';
    char opt8 = '8';
    char opt9 = '9';

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
    if (activePlayer == 1) ? drawSelection = 'X' : (drawSelection = 'O');
    
    if(selection != 0)
    {
        switch (selection)
        {
        case 1:
            opt1 = drawSelection;
            break;
        
        case 2:
            opt2 = drawSelection;
            break;
        
        case 3:
            opt3 = drawSelection;
            break;
        
        case 4:
            opt4 = drawSelection;
            break;
        
        case 5:
            opt5 = drawSelection;
            break;
        
        case 6:
            opt6 = drawSelection;
            break;
        
        case 7:
            opt7 = drawSelection;
            break;
        
        case 8:
            opt8 = drawSelection;
            break;
        
        case 9:
            opt9 = drawSelection;
            break;
        
        default:
            break;
        }
    }

    printf("\n\t  _____ _        _____            _____           ");
    printf("\n\t |_   _(_) ___  |_   _|_ _  ___  |_   _|__   ___  ");
    printf("\n\t   | | | |/ __|   | |/ _` |/ __|   | |/ _ \\ / _ \\ ");
    printf("\n\t   | | | | (__    | | (_| | (__    | | (_) |  __/ ");
    printf("\n\t   |_| |_|\\___|   |_|\\__,_|\\___|   |_|\\___/ \\___| ");
    printf("\n\n\tPlayer 1 is playing (X), Player 2 is playing (O)\n");
    printf("\n\t\t\t     |     |     ");
    printf("\n\t\t\t  %c  |  %c  |  %c  ", opt1, opt2, opt3);
    printf("\n\t\t\t_____|_____|_____");
    printf("\n\t\t\t     |     |     ");
    printf("\n\t\t\t  %c  |  %c  |  %c  ", opt4, opt5, opt6);
    printf("\n\t\t\t_____|_____|_____");
    printf("\n\t\t\t     |     |     ");
    printf("\n\t\t\t  %c  |  %c  |  %c  ", opt7, opt8, opt9);
    printf("\n\t\t\t     |     |     ");
    printf("\n\n\tPrlayer %d - select a position: ", activePlayer);

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
            if(boardArray[0][selection - 1] == 0)
            {
                (activePlayer == 1) ? boardArray[0][selection - 1] = 1 : (boardArray[0][selection - 1] = 2);
            }
            else
            {
                validFlag = 0;
            }
            break;
        
        case 4:
        case 5:
        case 6:
            if(boardArray[1][selection - 1] == 0)
            {
                (activePlayer == 1) ? boardArray[1][selection - 1] = 1 : (boardArray[1][selection - 1] = 2);
            }
            else
            {
                validFlag = 0;
            }
            break;
        
        case 7:
        case 8:
        case 9:
            if(boardArray[2][selection - 1] == 0)
            {
                (activePlayer == 1) ? boardArray[2][selection - 1] = 1 : (boardArray[2][selection - 1] = 2);
            }
            else
            {
                validFlag = 0;
            }
            break;

        default:
            validFlag = 0;
            break;
    }

    return validFlag;
}
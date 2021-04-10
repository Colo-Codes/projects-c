/*******************
 * By Damian Demasi
 * Date: 05/04/2021
 * Requirements: Create a "guess the number" game in C.
 *      The program must generate a random number between 0 and 20.
 *      Ask the user to guess the number.
 *          The user should only be able to enter numbers from 0 to 20.
 *      Give indications to the user if the entered number is too high or too low.
 *      The player wins the game if they can guess the number within 5 tries.
 *******************/

#include <stdio.h>
#include <stdlib.h> // For the random number
#include <time.h> // For the seed of the random number

int main()
{
    // Generate a random number between 0 and 20
    time_t t;
    srand((unsigned) time(&t)); // Initialization, should only be called once.
    int randNum = rand() % 21;
    
    // Greet the user
    // Ask the user for they chosen number
    // Warn they if the number is outside the 0 to 20 window

    // While the number is not the correct one,
    //      If the number is incorrect, ask again
    //      If the guessed number is higher, display "number too high" and ask again
    //      If the guessed number is lower, display "number too low" and ask again
    //      Count the number of tries
    //          If number of tries is over 5, game over


    printf("%d", randNum);



    return 0;
}
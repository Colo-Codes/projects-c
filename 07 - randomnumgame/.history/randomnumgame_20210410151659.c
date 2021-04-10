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

#define MAX_GUESSES 5
#define MIN_NUM 0
#define MAX_NUM 20

int main()
{
    unsigned int guessedNumber;
    unsigned int attempts = 1;
    time_t t;
    
    // Generate a random number between 0 and 20
    srand((unsigned) time(&t)); // Initialization, should only be called once.
    int randNum = rand() % (MAX_NUM + 1);
    
    // Greet the user
    printf("\nHi player! Please, guess an integer number between %d and %d: ", MIN_NUM, MAX_NUM);

    while (attempts <= MAX_GUESSES && guessedNumber != randNum)
    {
        // Ask the user for their chosen number
        scanf("%d", &guessedNumber);
        
        // Warn they if the number is outside the 0 to 20 window
        while (MAX_NUM < guessedNumber || guessedNumber < MIN_NUM)
        {
            printf("\nPlease, choose a number between %d and %d: ", MIN_NUM, MAX_NUM);
            scanf("%d", &guessedNumber);
        }

        // While the number is not the correct one,
        //      If the number is incorrect, ask again
        //      If the guessed number is higher, display "number too high" and ask again
        //      If the guessed number is lower, display "number too low" and ask again
        if (guessedNumber < randNum)
        {
            printf("\nYour guess is too low.");
        }
        else if (guessedNumber > randNum)
        {
            printf("\nYour guess is too high.");
        }
        else
        {
            printf("\nCONGRATULATIONS!!! You guessed correctly in %d attempt(s)! The number was: %d\n\n", attempts, randNum);
        }

        if (guessedNumber != randNum && attempts < MAX_GUESSES)
        {
            printf(" Try again (used %d out of %d attempts): ", attempts, MAX_GUESSES);
        }
        else if (guessedNumber != randNum && attempts >= MAX_GUESSES)
        {
            printf(" The number was %d. GAME OVER!\n\n", randNum);

        }

        //      Count the number of tries
        //          If number of tries is over 5, game over
        attempts++;
    }

    return 0;
}
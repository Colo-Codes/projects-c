/***********************
 * By Damian Demasi
 * Date: 10/04/2021
 * Requirements: create a program to print all prime numbers from 3 to 100.
 *      There will be no input.
 *      The output should be each prime number separated by a space on a single line.
 *      Use an array to store prime numbers.
 *      You can hard-code the first two prime numbers (2 and 3) on the array.
 *      Use loops.
 * Hints:
 *      Criteria to find a prime number:
 *          A number is considered prime if it is not evenly divisible by any other previous prime number.
 *          A test to ensure that a number n is prime, is to check n >= i * i, where i is a previous (prime) number.
 *          Even numbers after 2 are not prime.
 ***********************/

// NOTE:
// A prime number is a whole number, greater than or equal to 2, whose only factors are 1 and itself.
// So 2, 3, 5, 7, 11 are prime numbers whilst, 6 = 2 × 3 and 35 = 5 × 7 aren’t.

#include <stdio.h>

#define MAX 100
#define START 3

int main ()
{
    int i = 0, j = 0;
    int primes[MAX] = {2, 3}; // The rest are initialised to zero.
    int currentNumber = START;
    
    for (i = 2; i <= MAX; i++)
    {
        int primeFlag = 1;

        currentNumber += currentNumber + 1;

        // First prime number check
        for (j = 2; j < currentNumber; j++)
        {
            if (currentNumber % j == 0 && j != currentNumber)
                primeFlag = 0;
        }

        // Second prime number check
        for (j = 0; j < sizeof(primes); j++)
        {
            // if ((int) currentNumber / primes[j] < primes[j])
            // {
            //     primeFlag = 0;
            // }
        }


        // Include new prime number into array
        if (primeFlag == 1)
        {
            primes[i++] = currentNumber;
        }
        
    }

    for (i = 0; i < sizeof(primes); i++)
    {
        printf(" %d", primes[i]);
    }
    

    return 0;
}


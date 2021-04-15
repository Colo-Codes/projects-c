/***********************
 * By Damian Demasi
 * Date: 14/04/2021
 * Requirements:
 *      - Write 3 functions.
 *      One function should find the greatest common divisor of two non-negative integer values and to return the result.
 *      Another function should calculate the absolute value of a number. Should take a float as paramenter and return a float as well. 
 *          Test this function with ints and floats.
 *      The third function should compute the square number of a number. If a negative argument is passed, then a message is displayed
 *          and -1.0 should be returned. Should use the absolute number of the previous function.
 *      Test, test, test!
 ***********************/

#include <stdio.h>

int greatestCommonDivisor();

int main()
{

    greatestCommonDivisor();

    return 0;
}

int greatestCommonDivisor()
{
    int n1 = -1, n2 = -1, i, gcd;

    while (n1 < 0 || n2 < 0)
    {
        printf("Please, enter two non-negative integer: ");
        scanf("%d %d", &n1, &n2);
    }

    for(i=1; i <= n1 && i <= n2; ++i)
    {
        // Checks if i is factor of both integers
        if (n1 % i == 0 && n2 % i == 0)
            gcd = i;
    }

    printf("The G.C.D. of %d and %d is: %d\n", n1, n2, gcd);

    return gcd;
}

float absolute(float n)
{
    float abs;

    if (n < 0.0)
    {
        abs = -1.0 * n;
    }
    else
    {
        abs = n;
    }
        

    return abs;
}
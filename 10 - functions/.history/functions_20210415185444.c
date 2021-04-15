/***********************
 * By Damian Demasi
 * Date: 14/04/2021
 * Requirements:
 *      - Write 3 functions.
 *      One function should find the greatest common divisor of two non-negative integer values and to return the result.
 *      Another function should calculate the absolute value of a number. Should take a float as paramenter and return a float as well. 
 *          Test this function with ints and floats.
 *      The third function should compute the square root number of a number. If a negative argument is passed, then a message is displayed
 *          and -1.0 should be returned. Should use the absolute number of the previous function.
 *      Test, test, test!
 ***********************/

#include <stdio.h>

// Declarations
int greatestCommonDivisor();
float absolute(float n);
float square(float n);

// Functions
int main()
{
    float absInput, abs, sqInput, sqN;

    // Greatest Common Divisor (GCD)
    greatestCommonDivisor();

    // Absolute
    printf("Please, enter a number you wish to get the absolute value from: ");
    scanf("%f", &absInput);
    abs = absolute(absInput);
    printf("The absolute value is: %.2f\n", abs);

    //Square root value
    printf("Please, enter a number you wish to get the square value from: ");
    scanf("%f", &sqInput);
    sqN = square(sqInput);
    printf("The square value is: %.2f\n", sqN);

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

    (n < 0.0) ? abs = -1.0 * n : (abs = n);

    return abs;
}

float square(float n)
{
    float sqrt, temp;

    if (n < 0.0)
    {
        sqrt = -1;
        printf("The number must be non-negative\n");
    }
    else
    {
        sqrt = absolute(n);

        // store the half of the given number e.g from 256 => 128
        sqrt = n / 2;
        temp = 0;

        // Iterate until sqrt is different of temp, that is updated on the loop
        while(sqrt != temp){
            // initially 0, is updated with the initial value of 128
            // (on second iteration = 65)
            // and so on
            temp = sqrt;

            // Then, replace values (256 / 128 + 128 ) / 2 = 65
            // (on second iteration 34.46923076923077)
            // and so on
            sqrt = ( n/temp + temp) / 2;
        }
    }

    return sqrt;
}
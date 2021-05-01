/*************************
 * By Damian Demasi
 * Date: 01/05/2021
 *************************/

#include <stdio.h>

int main()
{
    // Save the number 25 in the 'number' variable
    int number = 25;

    // Save the address of the 'number' variable (&number) in the pointer 'pNumber'
    int *pNumber;
    pNumber = &number;

    // To access the actual value, we need to dereference the pointer
    printf("Printing *pNumber: %d\n", *pNumber); // Output: 25
    printf("Printing &pNumber: %d\n", &pNumber); // Output: 25
    printf("Printing pNumber:  %d\n", pNumber); // Output: 25

    return 0;
}
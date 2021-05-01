/*************************
 * By Damian Demasi
 * Date: 01/05/2021
 *************************/

#include <stdio.h>
#include <string.h> // For strtok()

int main()
{
    // Save the number 25 in the 'number' variable
    int number = 25;

    // Save the address of the 'number' variable (&number) in the pointer 'pNumber'
    int *pNumber;
    pNumber = &number;

    // To access the actual value, we need to dereference the pointer
    printf("Printing *pNumber: %d\n", *pNumber); // Output: 25
    printf("Printing &pNumber: %p\n", &pNumber); // Output: address of 'pNumber' pointer
    printf("Printing pNumber:  %p, %p\n", pNumber, &number); // Output: address of the 'number' variable

    char str[80] = "This is - www.tutorialspoint.com - website";
    const char s[2] = "-";
    char *token;

    printf("\n- - - Tokenizing - - -\n");
    // get the first token
    token = strtok(str, s);
    // walk through other tokens
    while( token != NULL ) {
        printf( " %s\n", token );
        token = strtok(NULL, s);
    }

    return 0;
}
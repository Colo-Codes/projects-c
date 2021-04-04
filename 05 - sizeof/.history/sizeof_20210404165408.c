/****************
 * By Damian Demasi
 * Date: 04/04/2021
 * Requirements: Create a C program that displays the byte size of basic data types supported in C.
 * **************/

#include <stdio.h>

int main ()
{
    printf("Size of int: \t\t\t%zd\n", sizeof(int));
    printf("Size of char: \t\t\t%zd\n", sizeof(char));
    printf("Size of long: \t\t\t%zd\n", sizeof(long));
    printf("Size of long long: \t\t\t%zd\n", sizeof(long long));
    printf("Size of double: \t\t%zd\n", sizeof(double));
    printf("Size of long double: \t\t%zd\n", sizeof(long double));

    return 0;
}
/******************************
* By Damian Demasi
* Date: 03/04/2021
* Requirements: Create a C program that defines an enum type and uses that type to display the values of some variables.
********************************/

#include <stdio.h>

int main()
{
    // Definition of enum
    enum company {GOOGLE, FACEBOOK, XEROX, YAHOO, EBAY, MICROSOFT};
    // Declaration of enum
    enum company myCompany1;
    enum company myCompany2;
    enum company myCompany3;

    // Value assignment
    myCompany1 = XEROX;
    myCompany2 = GOOGLE;
    myCompany3 = EBAY;

    // Output
    printf("The first company is %s, and its located in the position %d in the enum definition.\n", myCompany1, &myCompany1);
    printf("The second company is %s, and its located in the position %d in the enum definition.\n", myCompany2, &myCompany2);
    printf("The third company is %s, and its located in the position %d in the enum definition.\n", myCompany3, &myCompany3);

    return 0;
}
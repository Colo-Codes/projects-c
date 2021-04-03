/***********************
* By: Damian Demasi
* Date: 03/04/2021
* Requirements: Create a C program that displays the perimeter and area of a rectangle.
***********************/

#include <stdio.h>

#define MAX 32

int main()
{
    int width, height;
    char input[32];

    printf("Enter the width of the rectangle: ");
    fgets(input,MAX,stdin) && sscanf(input,"%d", &width);

    printf("Enter the height of the rectangle: ");
    fgets(input,MAX,stdin) && sscanf(input,"%d", &height);

    int perimeter = (width + height) * 2;
    int area = width * height;

    printf("\nThe rectangle has %d in width and %d in height.\n", width, height);
    printf("The perimeter is: %d\n", perimeter);
    printf("The area is: %d\n", area);

    return 0;
}
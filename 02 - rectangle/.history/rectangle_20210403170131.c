/***********************
* By: Damian Demasi
* Date: 03/04/2021
* Requirements: Create a C program that displays the perimeter and area of a rectangle.
***********************/

#include <stdio.h>

int main(int argc, char *argv[])
{
    int width, height;

    width = argv[1];
    height = argv[2];

    int perimeter = (width + height) * 2;
    int area = width * height;

    printf("The rectangle has %d in width and %d in height.\n", width, height);
    printf("The perimeter is: %d\n", perimeter);
    printf("The area is: %d\n", area);

    return 0;
}
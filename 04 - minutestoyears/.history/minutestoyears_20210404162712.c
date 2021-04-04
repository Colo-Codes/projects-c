/*****************
 * By Damian Demasi
 * Date: 04/04/2021
 * Requirements: Create a C program that converts the number of minutes to days and years.
 * **************/

#include <stdio.h>

int main ()
{
    int minutes;

    // 60 min in hour, 24 hours in day, 365 days in year (average)
    double minutesInADay = 60 * 24;
    double minutesInAYear = minutesInADay * 365;

    // request number of minutes
    printf("Enter the number of minutes: ");
    scanf("%d", &minutes);

    // number of minutes in a day
    printf("\n%d minutes are equal to %f days.", minutes, minutes / minutesInADay);

    // number of minutes in a year
    printf("\n%d minutes are equal to %f years.\n", minutes, (double) minutes / minutesInAYear);

    return 0;
}
/****************
 * By Damian Demasi
 * Date: 05/04/2021
 * Requirements: Calculate weekly pay.
 *      Basic pay rate: $12.00/h
 *      Overtime (more than 40 hours) = 1.5 pay rate.
 *      Tax rate:
 *          15% if <= $300
 *          20% if > $300 and <= $450
 *          25% if > $450
 * ****************/

#include <stdio.h>

int main()
{
    float weeklyHours = 0.0;

    // Get the number of weekly worked hours
    printf("Enter the number of weekly worked hours: ");
    scanf("%f", &weeklyHours);

    printf("\nTEST -> weeklyHours=%.2f", weeklyHours);

    return 0;
}
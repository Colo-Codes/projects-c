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

#define REGULAR_WEEKLY_HOURS 40

int main()
{
    float weeklyHours = 0.0, weeklyPay = 0.0, taxes = 0.0;

    // Get the number of weekly worked hours
    printf("Enter the number of weekly worked hours: ");
    scanf("%f", &weeklyHours);

    // Gross salary calculation
    if (weeklyHours <= REGULAR_WEEKLY_HOURS)
    {
        weeklyPay = weeklyHours * 12;
    }
    else
    {
        weeklyPay = (REGULAR_WEEKLY_HOURS * 12) + (weeklyHours - REGULAR_WEEKLY_HOURS) * (12 * 1.5);
    }

    // Tax calculation
    if (weeklyPay <= 300)
    {
        taxes = weeklyPay * 15 / 100;
    }
    else if (weeklyPay > 300 && weeklyPay <= 450)
    {
        taxes = weeklyPay * 20 / 100;
    }
    else
    {
        taxes = weeklyPay * 25 / 100;
    }

    // Output
    printf("\nYour gross salary is:\t$%.2f (%.2f + %.2f * %.2f)", weeklyPay, (weeklyHours * 12), (weeklyHours - REGULAR_WEEKLY_HOURS), (12 * 1.5));
    printf("\nYour total tax is:\t$%.2f", taxes);
    printf("\nYour net salary is:\t$%.2f\n", weeklyPay - taxes);

    return 0;
}
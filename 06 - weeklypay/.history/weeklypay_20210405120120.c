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
#define REGULAR_PAY 12
#define OVERTIME_MODIFIER 1.5
#define TAXES_LOWER_LIMIT 300
#define TAXES_MEDIUM_LIMIT 450
#define TAXES_FIRST_STEP 15
#define TAXES_SECOND_STEP 20
#define TAXES_THIRD_STEP 25

int main()
{
    float weeklyHours = 0.0, weeklyPay = 0.0, taxes = 0.0;

    // Get the number of weekly worked hours
    printf("Enter the number of weekly worked hours: ");
    scanf("%f", &weeklyHours);

    // Gross salary calculation
    if (weeklyHours <= REGULAR_WEEKLY_HOURS)
    {
        weeklyPay = weeklyHours * REGULAR_PAY;
    }
    else
    {
        weeklyPay = (REGULAR_WEEKLY_HOURS * REGULAR_PAY) + (weeklyHours - REGULAR_WEEKLY_HOURS) * (REGULAR_PAY * OVERTIME_MODIFIER);
    }

    // Tax calculation
    if (weeklyPay <= TAXES_LOWER_LIMIT)
    {
        taxes = weeklyPay * TAXES_FIRST_STEP / 100;
    }
    else if (weeklyPay > TAXES_LOWER_LIMIT && weeklyPay <= TAXES_MEDIUM_LIMIT)
    {
        taxes = weeklyPay * TAXES_SECOND_STEP / 100;
    }
    else
    {
        taxes = weeklyPay * TAXES_THIRD_STEP / 100;
    }

    // Output
    printf("\nYour gross salary is:\t$%.2f", weeklyPay);
    printf("\nYour total tax is:\t$%.2f", taxes);
    printf("\nYour net salary is:\t$%.2f\n", weeklyPay - taxes);

    return 0;
}
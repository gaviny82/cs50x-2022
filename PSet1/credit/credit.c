#include <cs50.h>
#include <stdio.h>
#include <math.h>

bool check_credit_card_number(long number);

int main(void)
{
    long number = get_long("Number: ");
    bool valid = check_credit_card_number(number);
    int length = ceil(log10(number)); // Number of digits
    int start = number / pow(10, length - 2);

    if (valid)
    {
        // Check the first two digits only when the number is valid
        if (length == 15 && (start == 34 || start == 37))
        {
            printf("AMEX\n");
        }
        else if (length == 16 && (start >= 51 && start <= 55))
        {
            printf("MASTERCARD\n");
        }
        else if ((length == 13 || length == 16) && (start >= 40 && start <= 49))
        {
            printf("VISA\n");
        }
        else
        {
            printf("INVALID\n");
        }
    }
    else
    {
        printf("INVALID\n");
    }
}

// Returns if the given number is a valid credit card number
bool check_credit_card_number(long number)
{
    if (number <= 0)
    {
        return false;
    }

    bool mult_by2 = false; // indicates if the current digit needs to be multiplied by 2
    int sum = 0;

    while (number != 0)
    {
        int digit = number % 10;
        number = floor(number / 10);

        // Each digit is obtained (starting from the last)
        if (mult_by2)
        {
            digit *= 2;

            // The maximum value of a digit multiplied by 2 is 18 = 9 * 2
            // When the result has two digits, add 1 to the sum, and 10 is subtracted from the result
            if (digit >= 10)
            {
                sum += 1;
                digit -= 10;
            }
            sum += digit;
        }
        else
        {
            sum += digit;
        }
        mult_by2 = !mult_by2;
    }

    return sum % 10 == 0;
}
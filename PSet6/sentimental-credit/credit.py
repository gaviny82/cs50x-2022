from cs50 import get_int
from math import ceil, log10


def main():

    # Require a number and convert to string
    number = str(get_int("Number: "))
    length = len(number)
    valid = check_credit_card_number(number)
    start = int(number[0:2])

    if valid:

        # Check the first two digits only when the number is valid
        if length == 15 and (start == 34 or start == 37):
            print("AMEX")
        elif length == 16 and (start >= 51 and start <= 55):
            print("MASTERCARD")
        elif (length == 13 or length == 16) and (start >= 40 and start <= 49):
            print("VISA")
        else:
            print("INVALID")

    else:
        print("INVALID")


# Returns if a number is valid
def check_credit_card_number(number):

    sum = 0

    # Add digits which should be multiplied by 2
    for digit in number[-2:0:2]:
        sum += int(digit) * 2

    # Add other digits
    for digit in number[-2:0:2]:
        sum += int(digit)

    return sum % 10 == 0


if __name__ == "__main__":
    main()

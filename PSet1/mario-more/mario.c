#include <cs50.h>
#include <stdio.h>

void print_chars(char ch, int num);

int main(void)
{
    // Get an input for height
    // Requires height > 0
    int height = 0;
    while (!(height >= 1 && height <= 8))
    {
        height = get_int("Height: ");
    }

    // Print the pyramids
    for (int line = 1; line <= height; line++)
    {
        // Print a line
        print_chars(' ', height - line);
        print_chars('#', line);
        print_chars(' ', 2);
        print_chars('#', line);
        //print_chars(' ', height - line);
        printf("\n");
    }
}

// Print the character ch for num times
void print_chars(char ch, int num)
{
    for (int i = 0; i < num; i++)
    {
        printf("%c", ch);
    }
}
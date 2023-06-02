#include <stdio.h>
#include <cs50.h>

int main(void)
{
    // Input name
    string name = get_string("Your name: ");

    // Output "Hello, <NAME>"
    printf("hello, %s\n", name);
}
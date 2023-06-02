#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int count_letters(string text);
int count_words(string text);
int count_sentences(string text);


int main(void)
{
    string text = get_string("Text: ");
    int letters = count_letters(text);
    int words = count_words(text);
    int sentences = count_sentences(text);

    // Compute sentences per 100 words
    float sentences_per_100_words = (float) sentences / (float) words * 100;
    // Compute letters per 100 words
    float letters_per_100_words = (float) letters / (float) words * 100;
    // Plug in values
    float cl_index = 0.0588 * letters_per_100_words - 0.296 * sentences_per_100_words - 15.8;
    int grade = round(cl_index);

    if (grade < 1)
    {
        printf("Before Grade 1\n");
    }
    else if (grade > 16)
    {
        printf("Grade 16+\n");
    }
    else
    {
        printf("Grade %i\n", grade);
    }

    //printf("Letters: %i\n", letters);
    //printf("Words: %i\n", words);
    //printf("Sentences: %i\n", sentences);
}

// Returns the number of letters in a text
int count_letters(string text)
{
    int count = 0;
    for (int i = 0, len = strlen(text); i < len; i++)
    {
        char ch = text[i];
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            count++;
        }
    }
    return count;
}

// Returns the number of sentences in a text
int count_words(string text)
{
    int len = strlen(text);

    // Returns 0 immediately if text is empty
    if (len == 0)
    {
        return 0;
    }

    // Init count to 1 word, increase by 1 for each ' ' charachter
    int count = 1;
    for (int i = 0; i < len; i++)
    {
        if (text[i] == ' ')
        {
            count++;
        }
    }
    return count;
}

// Returns the number of words in a text
int count_sentences(string text)
{
    int count = 0;
    for (int i = 0, len = strlen(text); i < len; i++)
    {
        char ch = text[i];
        if (ch == '.' || ch == '!' || ch == '?')
        {
            count++;
        }
    }
    return count;
}
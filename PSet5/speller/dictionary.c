// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <strings.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Creates a node
node *init_node(char *word, node *next)
{
    node *n = malloc(sizeof(node));
    strcpy(n->word, word);
    n->next = next;

    return n;
}

// Choose number of buckets in hash table
const unsigned int N = 2000;

// Hash table
node *table[N];
node *table_last[N]; // Pointer to the last element in each bucket

void append(int bucket, node *n)
{
    node *item = table[bucket];

    if (item == NULL) // No element at `bucket`
    {
        table[bucket] = n; // Append to the end
        table_last[bucket] = n; // Update the pointer to the last element
    }
    else
    {
        table_last[bucket]->next = n; // Append to the end
        table_last[bucket] = n; // Update the pointer to the last element
    }
}

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    int bucket = hash(word) % N;
    node *n = table[bucket];
    while (n != NULL)
    {
        if (strcasecmp(word, n->word) == 0)
        {
            return true;
        }
        n = n->next;
    }

    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    int hash = 0;

    char c;
    for (int i = 0; (c = word[i]) != '\0'; i++)
    {
        if (c != '\'')
        {
            hash += (tolower(c) - 'a' + 1) * i;
        }
        hash += 27;
        // 27 is the number of letters in the alphabet and apostrophe
    }

    return hash;
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    // Init hash table
    for (int i = 0; i < N; i++)
    {
        table[i] = NULL;
        table_last[i] = NULL;
    }

    // Open dictionary
    FILE *dict = fopen(dictionary, "r");
    if (dict == NULL)
    {
        return false;
    }

    // Read dictionary
    char word[LENGTH + 1];
    char c;
    int index = 0;
    while (fread(&c, sizeof(char), 1, dict) == 1)
    {
        if (c == '\n')
        {
            word[index] = '\0';
            index = 0;

            // Add word to hash table
            int bucket = hash(word) % N;
            append(bucket, init_node(word, NULL));
        }
        else if (isalpha(c) || c == '\'')
        {
            word[index] = c;
            index++;
        }
        else
        {
            return false;
        }
    }

    fclose(dict);
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    int count = 0;

    for (int i = 0; i < N; i++)
    {
        node *n = table[i];
        while (n != NULL)
        {
            count++;
            n = n->next;
        }
    }

    return count;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    for (int i = 0; i < N; i++)
    {
        node *n = table[i];
        while (n != NULL)
        {
            node *current = n;
            n = n->next;
            free(current);
        }
    }

    return true;
}

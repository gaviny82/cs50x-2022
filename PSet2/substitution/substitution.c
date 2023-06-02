#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

bool validate_key(string *key);
string encrypt(string plain, string key);


int main(int argc, string argv[])
{
    // Reject incorrect usage
    if (argc != 2)
    {
        printf("Usage: ./substitution key\n");
        return 1;
    }

    string key = argv[1];
    bool valid = validate_key(&key);
    if (!valid)
    {
        return 1;
    }

    string plain = get_string("plaintext: ");
    string cipher = encrypt(plain, key);
    printf("ciphertext: %s\n", cipher);

    return 0;
}

// Returns if the key is valid
// Parameter key is passed by reference, and all letters are converted to its upper case.
bool validate_key(string *key)
{
    // Reject invalid key: invalid length
    if (strlen(*key) != 26)
    {
        printf("Key must contain 26 characters.\n");
        return false;
    }

    bool flags[26] = {false};

    for (int i = 0; i < 26; i++)
    {
        char *ch = &(*key)[i];
        *ch = (char) toupper(*ch); // Convert all characters to upper case

        if (!(*ch >= 'A' && *ch <= 'Z'))
        {
            // Reject invalid key: invalid characters
            printf("Key must contain 26 letters only.\n");
            return false;
        }

        int index = *ch - 'A';
        if (flags[index])
        {
            // Reject invalid key: duplicate letters
            printf("Duplicate letters in the key.\n");
            return false;
        }
        else
        {
            flags[index] = true;
        }
    }

    return true;
}

// Return the cipher text using the key given
string encrypt(string plain, string key)
{
    for (int i = 0, length = strlen(plain); i < length; i++)
    {
        char *ch = &plain[i];
        if (*ch >= 'a' && *ch <= 'z')
        {
            *ch = key[*ch - 'a'] + ('a' - 'A');
        }
        else if (*ch >= 'A' && *ch <= 'Z')
        {
            *ch = key[*ch - 'A'];
        }
        else
        {
            // Ignore non-alphabetic character
            continue;
        }
    }

    return plain;
}
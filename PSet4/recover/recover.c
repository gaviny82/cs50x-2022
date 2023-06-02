#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cs50.h>

const int FAT_BLOCK_SIZE = 512; // bytes

// JPEG signature bytes
const uint8_t JPEG_SIG_BYTE0 = 0xff;
const uint8_t JPEG_SIG_BYTE1 = 0xd8;
const uint8_t JPEG_SIG_BYTE2 = 0xff;

bool check_jpeg_signature(uint8_t block[FAT_BLOCK_SIZE]);
string get_file_name(int id);

int main(int argc, char *argv[])
{
    // Check command-line arguments
    if (argc != 2)
    {
        printf("Usage: ./recover IMAGE\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        printf("Cannot open file %s.\n", argv[1]);
        return 1;
    }

    uint8_t buffer[FAT_BLOCK_SIZE];
    FILE *jpeg = NULL;
    int count = 0;

    while (fread(buffer, sizeof(uint8_t), FAT_BLOCK_SIZE, file) == FAT_BLOCK_SIZE)
    {
        // Detect beginning of a new JPEG image
        if (check_jpeg_signature(buffer))
        {
            // Close previous file
            if (jpeg != NULL)
            {
                fclose(jpeg);
            }

            // Construct file name
            char file_name[8] = "000.jpg";
            int digit1, digit10, digit100;
            digit1 = count % 10;
            digit10 = count / 10 % 10;
            digit100 = count / 100 % 10;
            count++;

            file_name[2] = digit1 + '0';
            file_name[1] = digit10 + '0';
            file_name[0] = digit100 + '0';

            // Create new file
            jpeg = fopen(file_name, "w");
            if (jpeg == NULL)
            {
                printf("Cannot create file %s.\n", file_name);
                return 2;
            }
        }

        // Write buffer to file
        if (jpeg != NULL)
        {
            int result = fwrite(buffer, sizeof(uint8_t), FAT_BLOCK_SIZE, jpeg);
            if (result != FAT_BLOCK_SIZE)
            {
                printf("Error in writing to a file.\n");
                return 3;
            }
        }
    }

    // Close current working file
    if (jpeg != NULL)
    {
        fclose(jpeg);
    }
    fclose(file);
}


// Returns if the block begins with a JPEG signature
bool check_jpeg_signature(uint8_t block[FAT_BLOCK_SIZE])
{
    return block[0] == JPEG_SIG_BYTE0 &&
           block[1] == JPEG_SIG_BYTE1 &&
           block[2] == JPEG_SIG_BYTE2 &&
           (block[3] >= 0b11100000 && block[3] <= 0b11101111);
}
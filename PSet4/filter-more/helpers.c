#include <math.h>
#include <stdio.h>

#include "helpers.h"

RGBTRIPLE average(RGBTRIPLE *pixels[], int count);
RGBTRIPLE sobel(RGBTRIPLE *pixels[3][3]);


// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            // Obtain pixel to be processed
            RGBTRIPLE *pixel = &image[y][x];
            int avg = round((pixel -> rgbtRed + pixel -> rgbtGreen + pixel -> rgbtBlue) / 3.0);
            pixel -> rgbtRed = avg;
            pixel -> rgbtGreen = avg;
            pixel -> rgbtBlue = avg;
        }
    }
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    int x_end = width / 2;
    for (int x = 0; x < x_end; x++)
    {
        for (int y = 0; y < height; y++)
        {
            RGBTRIPLE *pixel_l = &image[y][x];
            RGBTRIPLE *pixel_r = &image[y][width - 1 - x];

            // Swap the pair of pixels
            RGBTRIPLE tmp = *pixel_l;
            *pixel_l = *pixel_r;
            *pixel_r = tmp;
        }
    }
}

// Returns the average color based on a 3x3 block
RGBTRIPLE average(RGBTRIPLE *pixels[], int count)
{
    int avg_r = 0;
    int avg_g = 0;
    int avg_b = 0;

    for (int i = 0; i < count; i++)
    {
        RGBTRIPLE *pixel = pixels[i];
        avg_r += (*pixel).rgbtRed;
        avg_g += (*pixel).rgbtGreen;
        avg_b += (*pixel).rgbtBlue;
    }

    RGBTRIPLE avg;
    avg.rgbtRed = round((float) avg_r / count);
    avg.rgbtGreen = round((float) avg_g / count);
    avg.rgbtBlue = round((float) avg_b / count);
    return avg;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // Create a copy of the original image
    RGBTRIPLE copy[height][width];
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            copy[y][x] = image[y][x];
        }
    }

    // Process central pixels
    for (int x = 1; x < width - 1; x++)
    {
        for (int y = 1; y < height + 1; y++)
        {
            RGBTRIPLE *pixel_ptrs[9] =
            {
                &copy[y - 1][x - 1], &copy[y - 1][x], &copy[y - 1][x + 1],
                &copy[y][x - 1], &copy[y][x], &copy[y][x + 1],
                &copy[y + 1][x - 1], &copy[y + 1][x], &copy[y + 1][x + 1]
            };
            image[y][x] = average(pixel_ptrs, 9);
        }
    }

    // Process corner pixels
    RGBTRIPLE *top_left_pixel_ptrs[4] =
    {
        &copy[0][0], &copy[0][1],
        &copy[1][0], &copy[1][1]
    };
    image[0][0] = average(top_left_pixel_ptrs, 4);

    RGBTRIPLE *top_right_pixel_ptrs[4] =
    {
        &copy[0][width - 2], &copy[0][width - 1],
        &copy[1][width - 2], &copy[1][width - 1]
    };
    image[0][width - 1] = average(top_right_pixel_ptrs, 4);

    RGBTRIPLE *bottom_left_pixel_ptrs[4] =
    {
        &copy[height - 2][0], &copy[height - 2][1],
        &copy[height - 1][0], &copy[height - 1][1]
    };
    image[height - 1][0] = average(bottom_left_pixel_ptrs, 4);

    RGBTRIPLE *bottom_right_pixel_ptrs[4] =
    {
        &copy[height - 2][width - 2], &copy[height - 2][width - 1],
        &copy[height - 1][width - 2], &copy[height - 1][width - 1]
    };
    image[height - 1][width - 1] = average(bottom_right_pixel_ptrs, 4);

    // Process edge pixels
    for (int x = 1; x < width - 1; x++)
    {
        // Top edge
        RGBTRIPLE *top_pixel_ptrs[6] =
        {
            &copy[0][x - 1], &copy[0][x], &copy[0][x + 1],
            &copy[1][x - 1], &copy[1][x], &copy[1][x + 1]
        };
        image[0][x] = average(top_pixel_ptrs, 6);

        // Bottom edge
        RGBTRIPLE *bottom_pixel_ptrs[6] =
        {
            &copy[height - 2][x - 1], &copy[height - 2][x], &copy[height - 2][x + 1],
            &copy[height - 1][x - 1], &copy[height - 1][x], &copy[height - 1][x + 1]
        };
        image[height - 1][x] = average(bottom_pixel_ptrs, 6);
    }

    for (int y = 1; y < height - 1; y++)
    {
        // Left edge
        RGBTRIPLE *left_pixel_ptrs[6] =
        {
            &copy[y - 1][0], &copy[y - 1][1],
            &copy[y][0], &copy[y][1],
            &copy[y + 1][0], &copy[y + 1][1]
        };
        image[y][0] = average(left_pixel_ptrs, 6);

        // Right edge
        RGBTRIPLE *right_pixel_ptrs[6] =
        {
            &copy[y - 1][width - 2], &copy[y - 1][width - 1],
            &copy[y][width - 2], &copy[y][width - 1],
            &copy[y + 1][width - 2], &copy[y + 1][width - 1]
        };
        image[y][width - 1] = average(right_pixel_ptrs, 6);
    }
}

int CNT = 0;
// Apply the Sobel operator to a pixel
RGBTRIPLE sobel(RGBTRIPLE *pixels[3][3])
{
    // Kernels Gx and Gy
    const int Gx[3][3] =
    {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    const int Gy[3][3] =
    {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}
    };

    int rx = 0, gx = 0, bx = 0;
    int ry = 0, gy = 0, by = 0;

    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            RGBTRIPLE *pixel = pixels[y][x];

            rx += (*pixel).rgbtRed * Gx[y][x];
            gx += (*pixel).rgbtGreen * Gx[y][x];
            bx += (*pixel).rgbtBlue * Gx[y][x];

            ry += (*pixel).rgbtRed * Gy[y][x];
            gy += (*pixel).rgbtGreen * Gy[y][x];
            by += (*pixel).rgbtBlue * Gy[y][x];
        }
    }

    RGBTRIPLE result;
    int red = round(sqrt(pow(rx, 2) + pow(ry, 2)));
    int green = round(sqrt(pow(gx, 2) + pow(gy, 2)));
    int blue = round(sqrt(pow(bx, 2) + pow(by, 2)));

    result.rgbtRed = red > 255 ? 255 : red;
    result.rgbtGreen = green > 255 ? 255 : green;
    result.rgbtBlue = blue > 255 ? 255 : blue;
    return result;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // Create a copy of the original image
    RGBTRIPLE copy[height][width];
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            copy[y][x] = image[y][x];
        }
    }

    RGBTRIPLE black;
    black.rgbtRed = 0;
    black.rgbtGreen = 0;
    black.rgbtBlue = 0;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            // Default values
            RGBTRIPLE *pixel_ptrs[3][3] =
            {
                {&black, &black, &black},
                {&black, &black, &black},
                {&black, &black, &black}
            };

            if (x > 0 && y > 0) // Top left pixel
            {
                pixel_ptrs[0][0] = &copy[y - 1][x - 1];
            }
            if (y > 0) // Top central pixel
            {
                pixel_ptrs[0][1] = &copy[y - 1][x];
            }
            if (x < width - 1 && y > 0) // Top right pixel
            {
                pixel_ptrs[0][2] = &copy[y - 1][x + 1];
            }

            if (x > 0) // Central left pixel
            {
                pixel_ptrs[1][0] = &copy[y][x - 1];
            }
            pixel_ptrs[1][1] = &copy[y][x]; // Central pixel
            if (x < width - 1) // Central right pixel
            {
                pixel_ptrs[1][2] = &copy[y][x + 1];
            }

            if (x > 0 && y < height - 1) // Bottom left pixel
            {
                pixel_ptrs[2][0] = &copy[y + 1][x - 1];
            }
            if (y < height - 1) // Bottom central pixel
            {
                pixel_ptrs[2][1] = &copy[y + 1][x];
            }
            if (x < width - 1 && y < height - 1) // Bottom right pixel
            {
                pixel_ptrs[2][2] = &copy[y + 1][x + 1];
            }

            image[y][x] = sobel(pixel_ptrs);
        }
    }

}

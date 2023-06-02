from cs50 import get_int

while True:
    height = get_int("Height: ")

    # Check validity
    if height >= 1 and height <= 8:
        break

# Print the pyramid
for i in range(1, height + 1):
    print(" " * (height - i) + "#" * i + "  " + "#" * i)
from cs50 import get_string


def main():
    text = get_string("Text: ")
    letters = len([ch for ch in text if ch.isalpha()])
    words = len(text.split(' '))
    sentences = count_sentences(text)

    # Compute sentences per 100 words
    sentences_per_100_words = sentences / words * 100

    # Compute letters per 100 words
    letters_per_100_words = letters / words * 100

    # Plug in values
    cl_index = 0.0588 * letters_per_100_words - 0.296 * sentences_per_100_words - 15.8
    grade = round(cl_index)

    if grade < 1:
        print("Before Grade 1")
    elif grade > 16:
        print("Grade 16+")
    else:
        print(f"Grade {grade}")


# Returns the number of sentences in a text
def count_sentences(text):
    count = 0

    for ch in text:
        if ch in ['.', '!', '?']:
            count += 1

    return count


if __name__ == "__main__":
    main()
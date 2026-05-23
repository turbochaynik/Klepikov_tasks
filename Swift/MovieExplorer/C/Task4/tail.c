#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

enum
{
    DEFAULT_LINE_COUNT = 10
};

int parseFlag(char* argument, int* lineCount, int* signFlag)
{
    int i = 1;
    *lineCount = DEFAULT_LINE_COUNT;
    if (argument[0] == '-' || argument[0] == '+')
    {
        *signFlag = argument[0];
        int parsedValue = 0;

        for (i = 1; argument[i] != '\0'; ++i)
        {
            if (argument[i] > '9' || argument[i] < '0')
            {
                *signFlag = 0;
                return 0;
            }
            parsedValue = parsedValue * 10 + (argument[i] - '0');
        }

        *lineCount = parsedValue;
        return 1;
    }
    else
    {
        *signFlag = 0;
        return 0;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3)
    {
        fprintf(stderr, "Wrong number of arguments! Usage: [-<int> / +<int>] <filename>\n");
        return 1;
    }

    int signFlag = 0;
    int lineCount = DEFAULT_LINE_COUNT;
    errno = 0;

    parseFlag(argv[1], &lineCount, &signFlag);
    FILE* file = NULL;

    if (signFlag == 0)
    {
        if (argc != 2)
        {
            fprintf(stderr, "Wrong number of arguments! Usage: [-<int> / +<int>] <filename>\n");
            return 1;
        }

        file = fopen(argv[1], "r");
    }
    else
    {
        if (argc != 3)
        {
            fprintf(stderr, "Wrong number of arguments! Usage: [-<int> / +<int>] <filename>\n");
            return 1;
        }

        file = fopen(argv[2], "r");
    }

    if (file == NULL)
    {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        return 1;
    }

    int totalLines = 0;
    char character;
    int currentPosition = 0, i;
    for (i = 0; (character = fgetc(file)) != EOF; ++i)
    {
        if (character == '\n')
        {
            currentPosition = i;
            ++totalLines;
        }
    }
    if (currentPosition != i) 
        ++totalLines;

    fseek(file, 0, SEEK_SET);

    i = 0;
    currentPosition = 0;

    char* buffer = calloc(256, sizeof(char));
    if (buffer == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return 1;
    }

    if (signFlag == '+')
    {
        while (fgets(buffer + currentPosition, 256, file))
        {
            if (buffer == NULL)
                break;
            else if (fseek(file, -1, SEEK_CUR) == 0 && fgetc(file) != '\n') // If we got only a part of string, not a string of size 255 with \n
            {
                currentPosition += 256;
                char* tempBuffer = realloc(buffer, currentPosition + 256);
                if (tempBuffer == NULL)
                {
                    fprintf(stderr, "Memory reallocation failed\n");
                    free(buffer);
                    fclose(file);
                    return 1;
                }
                buffer = tempBuffer;
            }
            else
            {
                ++i;
                if (i >= lineCount)
                    printf("%s", buffer);

                free(buffer);
                currentPosition = 0;
                buffer = calloc(256, sizeof(char));
                if (buffer == NULL)
                {
                    fprintf(stderr, "Memory allocation failed\n");
                    fclose(file);
                    return 1;
                }
            }
        }
    }
    else
    {
        while (fgets(buffer + currentPosition, 256, file))
        {
            if (buffer == NULL)
                break;
            else if (ftell(file) % 256 == 0) {
                currentPosition += 256;
                char* tempBuffer = realloc(buffer, currentPosition + 256);
                if (tempBuffer == NULL)
                {
                    fprintf(stderr, "Memory reallocation failed\n");
                    free(buffer);
                    fclose(file);
                    return 1;
                }
                buffer = tempBuffer;
            }
            else
            {
                ++i;
                if (i >= (totalLines - lineCount))
                    printf("%s", buffer);

                free(buffer);
                currentPosition = 0;
                buffer = calloc(256, sizeof(char));
                if (buffer == NULL)
                {
                    fprintf(stderr, "Memory allocation failed\n");
                    fclose(file);
                    return 1;
                }
            }
        }
    }

    free(buffer);
    fclose(file);
}


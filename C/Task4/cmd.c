#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Wrong number of arguments! Usage: <file1> <file2>\n");
        return 1;
    }

    FILE* file1 = fopen(argv[1], "r");

    if (file1 == NULL)
    {
        fprintf(stderr, "Error opening the first file: %s\n", strerror(errno));
        return 1;
    }

    FILE* file2 = fopen(argv[2], "r");
    if (file2 == NULL)
    {
        fprintf(stderr, "Error opening the second file: %s\n", strerror(errno));
        fclose(file1);
        return 1;
    }

    int continueReading = 1;
    int currentLine = 1, currentByte = 0;
    int bytesRead1, bytesRead2;
    char charFromFile1 = '\0', charFromFile2 = '\0';

    while (continueReading)
    {
        currentByte += 1;
        bytesRead1 = fread(&charFromFile1, 1, 1, file1);
        bytesRead2 = fread(&charFromFile2, 1, 1, file2);

        if (bytesRead1 != bytesRead2)
        {
            if (bytesRead1 == 0)
            {
                if (charFromFile1 == '\n')
                    currentLine--;

                printf("Reached EOF in %s after byte %d, line %d\n", argv[1], currentByte-1, currentLine);
            }
            else if (bytesRead2 == 0)
            {
                if (charFromFile2 == '\n')
                    currentLine--;

                printf("Reached EOF in %s after byte %d, line %d\n", argv[2], currentByte-1, currentLine);
            }

            continueReading = 0;
        }
        else if (bytesRead1 == 0)
        {
            continueReading = 0;
        }
        else
        {
            if (charFromFile1 == charFromFile2)
            {
                if (charFromFile1 == '\n')
                    currentLine += 1;
            }
            else
            {
                printf("%s and %s differ at byte %d, line %d\n", argv[1], argv[2], currentByte, currentLine);
                continueReading = 0;
            }
        }
    }

    fclose(file1);
    fclose(file2);
}


/**
 * @file compilerTest.c
 *
 * @brief Implements test functions for compiler as whole.
 * 
 * Project: IFJ-2023
 *
 * @author Jan Seibert <xseibe00@stud.fit.vutbr.cz>
 */

#include <stdio.h>
#include <stdlib.h>

//#include "scanner.h"
#include "parser.h"
#include "generator.h"


#define PRINTF_ERR(fmt, ...)    \
    printf("\033[0;31m");       \
    printf(fmt, __VA_ARGS__);   \
    printf("\033[0m");

#define PRINTF_OK(fmt, ...)     \
    printf("\033[0;32m");       \
    printf(fmt, __VA_ARGS__);   \
    printf("\033[0m");

#define MAX_TESTS 100     
#define MAX_LINE_LENGTH 10000

int main()
{
    FILE *inputFile = fopen("test_input.txt", "r");
    if (inputFile == NULL)
    {
        PRINTF_ERR("%s\n", "File input.txt not found.");
        return 1;
    }

    char content[MAX_TESTS][MAX_LINE_LENGTH];
    int errCodes[MAX_TESTS];

    int fileIndex = 0;
    char line[MAX_LINE_LENGTH]; // Adjust the buffer size as needed

    // SEPARATE TO MULTIPLE FILES
    while (fgets(line, sizeof(line), inputFile) != NULL) {
        int errNumber;
        // Check if the line contains brackets with a number
        if (sscanf(line, "[%d]", &errNumber) == 1) {
            // Store the bracket number in the array
            errCodes[fileIndex++] = errNumber;
        } else if (strncmp(line, "---------------------------------------------------", 51) != 0) {
            // Continue filling the content array, excluding the separator line
            if (fileIndex > 0) {
                // Ensure the content does not exceed the allocated buffer size
                if (strlen(line) + strlen(content[fileIndex - 1]) < MAX_LINE_LENGTH) {
                    strcat(content[fileIndex - 1], line);
                }
            }
        }
    }

    int result;

    for (int i = 0; i < fileIndex; i++) 
    {
        printf("**********FILE[%d]**********\n", i);

        FILE *currentFile = fopen("temp.txt", "w");
        if (currentFile != NULL) 
        {
            fprintf(currentFile, "%s", content[i]);
            fclose(currentFile);
            currentFile = fopen("temp.txt", "r");

            if (currentFile != NULL)
            {
                ParserDataT gData;
                InitParserData(&gData);
                generator_start(gData.code);

                SetSourceFile(currentFile);

                result = loadFuns(&gData);
                if (result != 0)
                {
                    return result;
                }
                
                result = parse(&gData);
                
                generator_main_end(gData.code);

                FILE *file = fopen("IFJCode23.code", "w");
                FluidCreateOutput(gData.code, file);
                fclose(file);
                
                if (result == errCodes[i])
                {
                    PRINTF_OK("%s\n", "****SYNTACTIC TEST SUCCESS****");
                    PRINTF_OK("****EXPECTED = %d, RESULT = %d****\n\n", errCodes[i], result);
                }
                else
                {
                    PRINTF_ERR("%s\n", "****SYNTACTIC TEST FAILURE****");
                    PRINTF_ERR("****EXPECTED = %d, RESULT = %d****\n\n", errCodes[i], result);
                }
                if (result == 0)
                {
                    result = sem_anal(&gData);
                    if (result == errCodes[i])
                    {
                        PRINTF_OK("%s\n", "****SEMANTIC TEST SUCCESS****");
                        PRINTF_OK("****EXPECTED = %d, RESULT = %d****\n\n", errCodes[i], result);
                    }
                    else
                    {
                        PRINTF_ERR("%s\n", "****SEMANTIC TEST FAILURE****");
                        PRINTF_ERR("****EXPECTED = %d, RESULT = %d****\n\n", errCodes[i], result);
                    }
                }

                fclose(currentFile);
                DisposeParserData(&gData);
            } 
        }
    }

    fclose(inputFile);
}

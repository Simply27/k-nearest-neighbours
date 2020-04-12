/*
- array of vectors
- input tests
- ignore symbols
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

size_t get_data(float*** vector_array, FILE* data)
{   
    int buffer_size = 128;

    size_t vector_array_iter = 0;

    *vector_array = malloc(buffer_size * sizeof(float*));

    /* defined as 'a' since letters are ighnored by default */
    int c = 'a', prev = 'a', prev_2 = 'a', prev_3 = 'a';

    while(c != '\n')
    {
        size_t vector_iter = 0;

        float* vector = (float*) malloc(buffer_size * sizeof(float));

        while (c != '\n' && c != EOF)
        {
            size_t element_iter = 0;
            bool is_exp = false;

            char *vector_element = (char*) malloc(buffer_size * sizeof(char));

            do
            {
                prev_3 = prev_2;
                prev_2 = prev;
                prev = c;
                c = getc(data);

                if (((isdigit((char) prev_2))
                    && ((char) prev == '.' || (char) prev == 'e')
                    && isdigit((char) c) && !is_exp)
                    || (!(isdigit((char) prev_2)) && (char) prev == '-'
                    && isdigit((char) c)))
                {
                    vector_element[element_iter++] = (char) prev;
                    vector_element[element_iter++] = (char) c;

                    if (!is_exp)
                    {
                        is_exp = true;
                    }
                }
                else if (isdigit((char) c))
                {
                    vector_element[element_iter++] = (char) c;
                }
                

                if (element_iter % buffer_size == 0)
                {
                    vector_element = realloc(vector_element,
                                            (element_iter + buffer_size)
                                            * sizeof(char));
                }
            } while (c != ' ' && c != ',' && c != '\n' && c != EOF);

            vector_element[element_iter] = '\0';

            vector_element = realloc(vector_element,
                                    (strlen(vector_element) + 1) * sizeof(char));

            vector[vector_iter++] = strtof(vector_element, NULL);
            free(vector_element);

            if (vector_iter % buffer_size == 0)
            {
                vector = realloc(vector, 2 * vector_iter * sizeof(float));
            }
        }

        vector = realloc(vector, vector_iter * sizeof(float));

        (*vector_array)[vector_array_iter++] = vector;
        free(vector);

        if (vector_array_iter % buffer_size == 0)
        {
            *vector_array = realloc(*vector_array,
                                    2 * vector_array_iter * sizeof(float*));
        }
    }

    *vector_array = realloc(*vector_array, vector_array_iter * sizeof(float*));

    return vector_array_iter;
}

void file_to_vectors()
{
    FILE* data = fopen("data.txt", "r");
    float** vector_array;

    size_t vector_length = get_data(&vector_array, data);

    for (int i = 0; i < vector_length; ++i)
    {
        for (int j = 0; j < 3*sizeof(float); ++j)
        {
            printf("%f ", vector_array[i][j]);
        }
    }

    free(vector_array);
    fclose(data);
}

int main()
{
    file_to_vectors();
    return 0;
}
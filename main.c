/*
- reading file input
- finish printing
- remove unnecessary funct
- create safemalloc funct
- resize the window xD
- free vector
- tests
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

float* get_data(FILE** data)
{   
    int buffer_size = 128;

    float *vector = (float*) malloc(buffer_size * sizeof(float));
    if (vector == NULL)
    {
        fprintf(stderr, "Failed to allocate %zu bytes.\n",
                buffer_size * sizeof(float));
        return NULL;
    }

    int c = '0';
    size_t vector_iter = 0;

    while (c != '\n' && c != EOF)
    {
        size_t element_iter = 0;

        char *vector_element = (char*) malloc(buffer_size * sizeof(char));
        if (vector_element == NULL)
        {
            fprintf(stderr, "Failed to allocate %zu bytes.\n",
                    buffer_size * sizeof(char));
            return NULL;
        }

        while ((c = fgetc(*data)) != ' ' && c != '\n' && c != EOF)
        {
            if (isdigit((char) c))
            {
                vector_element[element_iter++] = (char) c;
            }
        }

        vector_element[element_iter] = '\0';

        vector_element = realloc(vector_element,
                                (strlen(vector_element) + 1) * sizeof(char));
        if (vector_element == NULL)
        {
            fprintf(stderr, "Failed to reallocate %zu bytes.\n",
                    buffer_size * sizeof(char));
            return NULL;
        }

        vector[vector_iter++] = strtof(vector_element, NULL);
        free(vector_element);
    }

    vector = realloc(vector, vector_iter * sizeof(float));
    if (vector == NULL)
    {
        fprintf(stderr, "Failed to reallocate %zu bytes.\n",
                vector_iter * sizeof(float));
        return NULL;
    }

    return vector;
}

void file_to_vectors()
{
    FILE *data = fopen("data.txt", "r");
    if (data)
    {
        float* f = get_data(&data);

        //for(int i=0; i<(sizeof(f)/sizeof(f[0])); ++i)
        //{
            printf("%f", *(++f));
        //}
    }

    fclose(data);
}

int main()
{
    file_to_vectors();
    return 0;
}
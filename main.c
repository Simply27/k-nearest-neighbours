/*
- warn about typos
- compare size of vectors
- input tests
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

typedef struct data_info
{
    size_t varray_size, vector_size;
    int c;
} data_info;

int get_data_member(char** vector_element, FILE* data)
{
    int buffer_size = 128;

    /* initialized as 'a' since letters are ignored by default */
    int c = 'a', prev = 'a', prev_2 = 'a', prev_3 = 'a';

    size_t element_iter = 0;
    bool is_exp = false;

    *vector_element = (char*) malloc(buffer_size * sizeof(char));

    while (c != ' ' && c != ',' && c != '\n' && c != EOF)
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
            (*vector_element)[element_iter++] = (char) prev;
            (*vector_element)[element_iter++] = (char) c;

            if (!is_exp)
            {
                is_exp = true;
            }
        }
        else if (isdigit((char) c))
        {
            (*vector_element)[element_iter++] = (char) c;
        }
        

        if (element_iter % buffer_size == 0)
        {
            *vector_element = realloc(*vector_element,
                                    (element_iter + buffer_size)
                                    * sizeof(char));
        }
    }

    (*vector_element)[element_iter] = '\0';

    *vector_element = realloc(*vector_element,
                            (strlen(*vector_element) + 1) * sizeof(char));
    
    return c;
}

data_info get_vector(float** vector, FILE* data)
{   
    int buffer_size = 128;

    /* initialized as 'a' since letters are ignored by default */
    int c = 'a';

    size_t vector_iter = 0;

    *vector = (float*) malloc(buffer_size * sizeof(float));

    while (c != '\n' && c != EOF)
    {
        char* vector_element;
        c = get_data_member(&vector_element, data);

        (*vector)[vector_iter++] = strtof(vector_element, NULL);
        free(vector_element);

        if (vector_iter % buffer_size == 0)
        {
            *vector = realloc(*vector, 2 * vector_iter * sizeof(float));
        }
    }

    *vector = realloc(*vector, vector_iter * sizeof(float));

    data_info info = { sizeof(float*), vector_iter, c };

    return info;
}

data_info get_varray(float*** varray, FILE* data)
{
    int buffer_size = 128;

    /* c initialized as 'a' since letters are ignored by default */
    data_info info = {sizeof(float*), sizeof(float), 'a'};

    size_t varray_iter = 0;

    *varray = (float**) malloc(buffer_size * sizeof(float*));

    while(info.c != EOF)  
    {
        float* vector;
        info = get_vector(&vector, data);

        (*varray)[varray_iter++] = vector;

        if (varray_iter % buffer_size == 0)
        {
            *varray = (float**) realloc(*varray,
                                        2 * varray_iter * sizeof(float*));
        }
    }

    *varray = (float**) realloc(*varray, varray_iter * sizeof(float*));

    info.varray_size = varray_iter;
    
    return info;
}

void file_to_vectors()
{
    FILE* data = fopen("data.txt", "r");
    float ** varray;

    data_info info = get_varray(&varray, data);

    for (size_t i = 0; i < info.varray_size; ++i)
    {
        for (size_t j = 0; j < info.vector_size; ++j)
        {
            printf("%f ", varray[i][j]);
        }
        printf("\n");
    }

    for (size_t i = 0; i < info.varray_size; ++i)
    {
        free(varray[i]);
    }
    
    free(varray);
    fclose(data);
}

int main()
{
    file_to_vectors();
    return 0;
}
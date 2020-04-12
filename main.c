/*
TODO

Input:
- error -> inputting 0 when space at the end or begining of vector
- cast to char macro?
- file to vectors is a temporary method
- input tests
- ask for vector
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

typedef enum read_error
{
    NO_ERROR,
    LETTER_AFTER_NUM,
    DIFFERENT_VECTOR_DIMENSION
} read_error;

typedef struct data_info
{
    size_t varray_size, vector_size;
    int c;
    read_error error;
} data_info;

void check_for_errors(int err)
{
    if (err == LETTER_AFTER_NUM)
    {
        printf("It looks like there are some typos in your data, "
               "which were ignored. The calculation will proceed, but "
               "please check your input to ensure the results are not flawed."
               "\n\nPress any key to continue...");
        getchar();
    }
    else if (err == DIFFERENT_VECTOR_DIMENSION)
    {
        printf("The vectors are not of the same dimension. "
               "Please check your input."
               "\nThe program will now terminate."
               "\n\nPress any key to continue...");
        getchar();
        exit(EXIT_FAILURE);
    }
}

data_info get_data_member(char** vector_element, FILE* data)
{
    int buffer_size = 128;

    data_info info = {sizeof(float*), sizeof(float), 'a', NO_ERROR};

    /* initialized as 'a' since letters are ignored by default */
    int prev = 'a', prev_2 = 'a', prev_3 = 'a';

    size_t element_iter = 0;
    bool is_exp = false, was_digit = false;

    *vector_element = (char*) malloc(buffer_size * sizeof(char));

    while (info.c != ' ' && info.c != ',' && info.c != '\n' && info.c != EOF)
    {
        prev_3 = prev_2;
        prev_2 = prev;
        prev = info.c;
        info.c = getc(data);

        if ((isdigit((char) prev_2)
            && (prev == '.' || prev == 'e')
            && isdigit((char) info.c) && !is_exp)
            || (!isdigit((char) prev_2) && prev == '-'
            && isdigit((char) info.c)))
        {
            (*vector_element)[element_iter++] = (char) prev;
            (*vector_element)[element_iter++] = (char) info.c;

            if (!is_exp)
            {
                is_exp = true;
            }
        }
        else if (!isdigit((char) info.c) && info.c != 'e' && info.c != ' '
                 && info.c != '\n' && info.c != EOF)
        {
            info.error = LETTER_AFTER_NUM;
        }
        else if (isdigit((char) info.c))
        {
            (*vector_element)[element_iter++] = (char) info.c;
            was_digit = true;
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
    
    return info;
}

data_info get_vector(float** vector, FILE* data)
{   
    int buffer_size = 128;

    /* initialized as 'a' since letters are ignored by default */
    data_info info = {sizeof(float*), sizeof(float), 'a', NO_ERROR};

    size_t vector_iter = 0;

    *vector = (float*) malloc(buffer_size * sizeof(float));

    while (info.c != '\n' && info.c != EOF)
    {
        char* vector_element;
        info = get_data_member(&vector_element, data);

        (*vector)[vector_iter++] = strtof(vector_element, NULL);
        free(vector_element);

        if (vector_iter % buffer_size == 0)
        {
            *vector = realloc(*vector, 2 * vector_iter * sizeof(float));
        }
    }

    *vector = realloc(*vector, vector_iter * sizeof(float));

    info.vector_size = vector_iter;

    return info;
}

data_info get_varray(float*** varray, FILE* data)
{
    int buffer_size = 128;

    /* c initialized as 'a' since letters are ignored by default */
    data_info info = {sizeof(float*), sizeof(float), 'a', NO_ERROR};

    size_t varray_iter = 0, prev_vector_size;

    *varray = (float**) malloc(buffer_size * sizeof(float*));

    while(info.c != EOF)  
    {
        float* vector;
        info = get_vector(&vector, data);

        if ((varray_iter != 0) && (info.vector_size != prev_vector_size))
        {
            info.error = DIFFERENT_VECTOR_DIMENSION;
        }

        prev_vector_size = info.vector_size;

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

    check_for_errors(info.error);

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
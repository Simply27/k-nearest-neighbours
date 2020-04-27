/*
TODO
- change floats to doubles
- secure correlation distance from dividing by 0
- change k checking in case of choosing a vector from file
- can conditions in get_data_member be written in different order? (refactoring)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

#define LARGE_BUFFER 128
#define SMALL_BUFFER 32

typedef enum read_error
{
    NO_ERROR,
    LETTER_AFTER_NUM,
    FLOAT_NOT_REPRESENTABLE,
    DIFFERENT_VECTOR_DIMENSION,
    EMPTY_FILE,
    NO_FILE
} read_error;

typedef enum distance
{
    EUCLIDEAN,
    CITY_BLOCK,
    CORRELATION
} distance;

void print_error(int err)
{
    if (err == LETTER_AFTER_NUM)
    {
        printf("It looks like there are some typos in your data, "
               "which were ignored. The calculation will proceed, but "
               "please check your input to ensure the results are not flawed.\n\n"
               "Press [Enter] to continue...");
        getchar();
    }
    else if (err == FLOAT_NOT_REPRESENTABLE)
    {
        printf("Some of values given are not representable by the float type "
               "Please check your input.\n"
               "The program will now terminate.\n\n"
               "Press [Enter] to continue...");
        getchar();
        exit(EXIT_FAILURE);
    }
    else if (err == DIFFERENT_VECTOR_DIMENSION)
    {
        printf("The vectors are not of the same dimension. "
               "Please check your input.\n"
               "The program will now terminate.\n\n"
               "Press [Enter] to continue...");
        getchar();
        exit(EXIT_FAILURE);
    }
    else if (err == EMPTY_FILE)
    {
        printf("Couldn't find any vectors in your input file. "
               "Please ensure it's filled properly.\n"
               "The program will now terminate.\n\n"
               "Press [Enter] to continue...");
        getchar();
        exit(EXIT_FAILURE);
    }
    else if (err == NO_FILE)
    {
        printf("Couldn't find the input file. "
               "Please ensure it's in the proper location.\n"
               "The program will now terminate.\n\n"
               "Press [Enter] to continue...");
        getchar();
        exit(EXIT_FAILURE);
    }
}

long int getlint(FILE* stream)
{
    char* buffer;
    size_t buf_size = SMALL_BUFFER;
    buffer = (char*) malloc(buf_size * sizeof(char));

    getline(&buffer, &buf_size, stream);
    long int num = strtol(buffer, NULL, 0);

    free(buffer);

    return num;
}

read_error get_data_member(char** vector_element, FILE* data, bool *was_digit,
                          int* c)
{
    int buffer_size = LARGE_BUFFER;

    /* initialized as 'a' since letters are ignored by default */
    int prev = 'a', prev_2 = 'a', prev_3 = 'a';
    *c = 'a';

    read_error error = NO_ERROR;
    size_t element_iter = 0;
    bool is_exp = false;

    *vector_element = (char*) malloc(buffer_size * sizeof(char));

    while (*c != ' ' && *c != ',' && *c != '\n' && *c != EOF)
    {
        prev_3 = prev_2;
        prev_2 = prev;
        prev = *c;
        *c = getc(data);

        if ((isdigit((char) prev_2) && (prev == '.') && isdigit((char) *c))
            || (isdigit((char) prev_2) && (prev == 'e')
            && (isdigit((char) *c) || *c == '-') && !is_exp)
            || ((!isdigit((char) prev_2) && prev_2 != 'e') && prev == '-'
            && isdigit((char) *c)))
        {
            (*vector_element)[element_iter++] = (char) prev;
            (*vector_element)[element_iter++] = (char) *c;

            if (!is_exp)
            {
                is_exp = true;
            }
        }
        else if (!isdigit((char) *c) && *c != '-' && *c != '.'
                 && *c != 'e' && *c != ' ' && *c != ','
                 && *c != '\n' && *c != EOF && *was_digit)
        {
            error = LETTER_AFTER_NUM;
        }
        else if (isdigit((char) *c))
        {
            (*vector_element)[element_iter++] = (char) *c;
            *was_digit = true;
        }

        if (element_iter != 0 && element_iter % buffer_size == 0)
        {
            *vector_element = realloc(*vector_element,
                                    (element_iter + buffer_size)
                                    * sizeof(char));
        }
    }

    (*vector_element)[element_iter] = '\0';

    *vector_element = realloc(*vector_element,
                            (strlen(*vector_element) + 1) * sizeof(char));
    
    return error;
}

read_error get_vector(float** vector, FILE* data, int* c)
{
    int buffer_size = LARGE_BUFFER;

    /* initialized as 'a' since letters are ignored by default */
    *c = 'a';

    /* first three members are reserved for the vecor dimension,
     data.txt file line and distnce */
    size_t vector_iter = 3;
    read_error error = NO_ERROR;
    bool was_digit = false;

    *vector = (float*) malloc(buffer_size * sizeof(float));

    while (*c != '\n' && *c != EOF)
    {
        char* vector_element;
        float temp_element;

        error = get_data_member(&vector_element, data, &was_digit, c);

        if (*vector_element != '\0')
        {
            if ((temp_element = strtof(vector_element, NULL)) == HUGE_VALF)
            {
                print_error(FLOAT_NOT_REPRESENTABLE);
            }

            (*vector)[vector_iter++] = temp_element;
        }

        free(vector_element);

        if (vector_iter % buffer_size == 0)
        {
            *vector = realloc(*vector, 2 * vector_iter * sizeof(float));
        }
    }

    *vector = realloc(*vector, vector_iter * sizeof(float));

    (*vector)[0] = (float) vector_iter;

    return error;
}

size_t get_varray(float*** varray, FILE* data)
{
    int buffer_size = LARGE_BUFFER;

    /* c initialized as 'a' since letters are ignored by default */
    int c = 'a';

    size_t varray_iter = 0, prev_vector_size;
    read_error error = NO_ERROR;
    float line_counter = 0;

    *varray = (float**) malloc(buffer_size * sizeof(float*));

    while(c != EOF)  
    {
        ++line_counter;

        float* vector;
        error = get_vector(&vector, data, &c);

        if (varray_iter != 0 && vector[0] != prev_vector_size && vector[0] != 3)
        {
            print_error(DIFFERENT_VECTOR_DIMENSION);
        }

        if (vector[0] != 3)
        {
            prev_vector_size = vector[0];
            vector[1] = line_counter;
            (*varray)[varray_iter++] = vector;
        }

        if (varray_iter != 0 && varray_iter % buffer_size == 0)
        {
            *varray = (float**) realloc(*varray,
                                        2 * varray_iter * sizeof(float*));
        }
    }

    *varray = (float**) realloc(*varray, varray_iter * sizeof(float*));

    if (varray_iter == 0)
    {
        error = EMPTY_FILE;
    }
    
    if (error)
    {
        print_error(error);
    }

    return varray_iter;
}

float vector_length(float* vector)
{
    float sum = 0;

    for (size_t i = 3; i < vector[0]; ++i)
    {
        sum += pow((vector[i]), 2);
    }

    return sqrtf(sum);
}

float dot_product(float* vector_1, float* vector_2)
{
    if (vector_1[0] != vector_2[0])
    {
        print_error(DIFFERENT_VECTOR_DIMENSION);
    }

    float sum = 0;

    for (size_t i = 3; i < vector_1[0]; ++i)
    {
        sum += vector_1[i] * vector_2[i];
    }
    
    return sum;
}

float vector_mean(float* vector)
{
    float sum = 0;

    for (size_t i = 3; i < vector[0]; ++i)
    {
        sum += vector[i];
    }
    
    return (sum/(vector[0]-3));
}

float* centred_vector(float* vector)
{
    float* temp_vector = malloc(vector[0] * sizeof(float));

    for (size_t i = 0; i < vector[0]; ++i)
    {
        temp_vector[i] = vector[i];
    }
    
    for (size_t i = 3; i < temp_vector[0]; ++i)
    {
        temp_vector[i] -= vector_mean(vector);
    }

    return temp_vector;
}

float correlation_distance(float* vector_1, float* vector_2)
{
    float* centred_vector_1 = centred_vector(vector_1);
    float* centred_vector_2 = centred_vector(vector_2);
    
    float cdist = 1 - (dot_product(centred_vector_1, centred_vector_2)
                      / (vector_length(centred_vector_1)
                         * vector_length(centred_vector_2)));
    
    free(centred_vector_1);
    free(centred_vector_2);
    return cdist;
}

float euclidean_distance(float* vector_1, float* vector_2)
{
    float sum = 0;

    for (size_t i = 3; i < vector_1[0]; ++i)
    {
        sum += pow((vector_1[i] - vector_2[i]), 2);
    }

    return sqrtf(sum);
}

float city_block_distance(float* vector_1, float* vector_2)
{
    float sum = 0;

    for (size_t i = 3; i < vector_1[0]; ++i)
    {
        sum += fabsf(vector_1[i] - vector_2[i]);
    }

    return sum;
}

int varray_sort(const void* a, const void* b)
{
    const float x = ((float*) *(float**)a)[2];
    const float y = ((float*) *(float**)b)[2];

    if (x < y)
    {
        return -1; 
    }
    else if (x > y)
    {
        return 1; 
    }
    
    return 0;
}

void print_neighbours(float** varray, size_t vector_size, long int k,
                      bool vector_in_file)
{
    size_t a;

    if (vector_in_file)
    {
        a = 1;
        ++k;
    }
    else
    {
        a = 0;
    }
    
    printf("\n");
    for (size_t i = a; i < k; ++i)
    {
        for (size_t j = 3; j < vector_size; ++j)
        {
            printf("%f ", varray[i][j]);
        }
        printf("  Distance: %f  At file line: %.0f\n", varray[i][2],
               varray[i][1]);
    }
    printf("\n");
}

long int get_k(size_t varray_size)
{
    long int k;

    while(1)
    {
        printf("\nHow many nearest neighbours would you like to print? ");
        k = getlint(stdin);
        
        if (k)
        {
            if (k <= varray_size)
            {
                return k;
            }
            else
            {
                printf("\nYour k is too big for the data you specified. "
                       "Please choose another one.\n\n");
            }
        }
        else
        {
            printf("\nLooks like there's something wrong with your input.\n"
                   "Please try one more time.\n\n");   
        }
    }
}

void calculate_distances(distance distance_measure, size_t varray_size,
                         float*** varray, float* user_vector)
{
    printf("\nCalculating distances...\n");
    for (size_t i = 0; i < varray_size; ++i)
    {   
        if (distance_measure == EUCLIDEAN)
        {
            (*varray)[i][2] = euclidean_distance((*varray)[i], user_vector);
        }
        else if (distance_measure == CITY_BLOCK)
        {
            (*varray)[i][2] = city_block_distance((*varray)[i], user_vector);
        }
        else if (distance_measure == CORRELATION)
        {
            (*varray)[i][2] = correlation_distance((*varray)[i], user_vector);
        }
    }
    printf("\nDone\n");
}

float* get_user_vector(size_t vector_size)
{
    float* user_vector;

    printf("\nPlease give me your vector "
           "(separate elements with spaces or commas): ");

    int dummy;
    read_error error = get_vector(&user_vector, stdin, &dummy);
                
    if (user_vector[0] != vector_size)
    {
        error = DIFFERENT_VECTOR_DIMENSION;
    }

    if (error)
    {
        printf("\nFollowing problem was found in your input vector:\n\n");
        print_error(error);
        printf("\n");
    }

    return user_vector;
}

size_t get_file_vector_pos(float** varray, size_t varray_size)
{
    long int row_number;

    while(1)
    {
        printf("\nPlease specify the file row with the desired vector: ");
        row_number = getlint(stdin);

        if (row_number)
        {
            for (size_t i = 0; i < varray_size; ++i)
            {
                if (varray[i][1] == row_number)
                {
                    return i;
                }
            }

            printf("\nNo vector in such row. "
                   "Please choose another one.\n\n");
        }
        else
        {
            printf("\nLooks like there's something wrong with your input.\n"
                   "Please try one more time.\n\n");   
        }
    }
}

bool user_vector_in_file()
{
    while (1)
    {
        printf("\nHow would you like to specify the reference vector?\n\n"
               "1) Define a vector\n"
               "2) Choose a line from the data file\n\n"
               "Choose your action: ");

        long int choice = getlint(stdin);
        if (choice < 1 || choice > 2)
        {
            choice = 0;
        }

        switch(choice)
        {
            case 1:
                return false;
            case 2:
                return true;
            default:
                printf("\nSeems like your input is incorrect. "
                       "Please try one more time.\n\n");
                break;
        }
    }
}

distance choose_distance_measure()
{
    while (1)
    {
        printf("\nWhich distance measure would you like to use in your "
               "calculations?\n\n"
               "1) Euclidean\n"
               "2) City-block\n"
               "3) Correlation\n\n"
               "Choose your action: ");

        long int choice = getlint(stdin);
        if (choice < 1 || choice > 3)
        {
            choice = 0;
        }
        
        switch(choice)
        {
            case 1:
                printf("\nDistance measure was changed to euclidean.\n");
                return EUCLIDEAN;
            case 2:
                printf("\nDistance measure was changed to city-block.\n");
                return CITY_BLOCK;
            case 3:
                printf("\nDistance measure was changed to correlation.\n");
                return CORRELATION;
            default:
                printf("\nSeems like your input is incorrect. "
                       "Please try one more time.\n\n");
                break;
        }
    }
}

size_t read_from_file(char* adress, float*** varray)
{
    printf("\nReading input file...\n");

    FILE* data = fopen(adress, "r");
    if (data == NULL)
    {
        printf("Following problem was found with your input data file:\n\n");
        print_error(NO_FILE);
    }

    size_t varray_size = get_varray(varray, data);

    fclose(data);
    printf("\nDone.\n");
    
    return varray_size;
}

int main()
{
    printf("\nK-Nearest-Neighbours Calculator\n"
        "-------------------------------\n");

    float** varray;
    size_t varray_size = read_from_file("data//data.txt", &varray);    

    distance distance_measure = choose_distance_measure();
    
    bool vector_in_file = user_vector_in_file();
    if (vector_in_file)
    {
        size_t user_vector_pos = get_file_vector_pos(varray, varray_size);
        calculate_distances(distance_measure, varray_size, &varray,
                            varray[user_vector_pos]);
    }
    else
    {
        float* user_vector = get_user_vector(varray[0][0]);
        calculate_distances(distance_measure, varray_size, &varray, user_vector);
        free(user_vector);
    }

    qsort(varray, varray_size, sizeof(float*), varray_sort);

    long int k = get_k(varray_size);

    print_neighbours(varray, varray[0][0], k, vector_in_file);

    for (size_t i = 0; i < varray_size; ++i)
    {
        free(varray[i]);
    }
    
    free(varray);
    
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

#define LARGE_BUFFER 128
#define SMALL_BUFFER 32

typedef enum input_error
{
    NO_ERROR,
    LETTER_AFTER_NUM,
    DOUBLE_NOT_REPRESENTABLE,
    DIFFERENT_VECTOR_DIMENSION,
    EMPTY_FILE,
    NO_FILE,
    CORRELATION_IMPOSSIBLE
} input_error;

typedef enum distance
{
    EUCLIDEAN,
    CITY_BLOCK,
    CORRELATION
} distance;

typedef struct previous_chars
{
    int first, second, third;
} previous_chars;


// INPUT

long int getlint(FILE* stream)
{
    char buffer[SMALL_BUFFER];

    fgets(buffer, SMALL_BUFFER, stream);
    long int num = strtol(buffer, NULL, 0);

    return num;
}

input_error scan_input(char** vector_element, size_t* element_iter, int c,
                      previous_chars prev, bool* was_digit, bool* was_exp)
{
    input_error error = NO_ERROR;

    bool was_dot = isdigit((char) prev.second)
                    && (prev.first == '.')
                    && isdigit((char) c);

    bool is_exp = isdigit((char) prev.second)
                    && (prev.first == 'e')
                    && (isdigit((char) c) || c == '-')
                    && !(*was_exp);

    bool was_negative = ((!isdigit((char) prev.second && !(*was_digit)))
                        || prev.second == ' ' || prev.second == ',')
                        && prev.second != 'e'
                        && prev.first == '-'
                        && isdigit((char) c);

    bool typo = !isdigit((char) c)
                && c != '-' && c != '.' && c != 'e' && c != ' ' && c != ','
                && c != '\n' && c != EOF
                && (*was_digit);

    bool typo_2 = !isdigit((char) prev.second) && !isdigit((char) c)
                  && (prev.first == '-' ||  prev.first == '.'
                  || prev.first == 'e' || prev.first == ' '
                  || prev.first == ',')
                  && (*was_digit);

    if (was_dot || is_exp || was_negative)
    {
        (*vector_element)[(*element_iter)++] = (char) prev.first;
        (*vector_element)[(*element_iter)++] = (char) c;

        if (!(*was_exp))
        {
            *was_exp = true;
        }
    }
    else if (typo || typo_2)
    {
        error = LETTER_AFTER_NUM;
    }
    else if (isdigit((char) c))
    {
        (*vector_element)[(*element_iter)++] = (char) c;
        *was_digit = true;
    }

    return error;
}

input_error get_data_member(char** vector_element, FILE* data, bool *was_digit,
                          int* c)
{
    int buffer_size = LARGE_BUFFER;

    /* initialized as 'a' since letters are ignored by default */
    previous_chars prev = {'a', 'a', 'a'};
    *c = 'a';

    input_error error = NO_ERROR, temp_error = NO_ERROR;
    size_t element_iter = 0;
    bool was_exp = false;

    *vector_element = (char*) malloc(buffer_size * sizeof(char));

    while (*c != ' ' && *c != ',' && *c != '\n' && *c != EOF)
    {
        prev.third = prev.second;
        prev.second = prev.first;
        prev.first = *c;
        *c = getc(data);

        temp_error = scan_input(vector_element, &element_iter, *c, prev,
                                was_digit, &was_exp);
        if(temp_error)
        {
            error = temp_error;
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

input_error get_vector(double** vector, FILE* data, int* c)
{
    int buffer_size = LARGE_BUFFER;

    /* initialized as 'a' since letters are ignored by default */
    *c = 'a';

    /* first three members are reserved for the vecor dimension,
     data.txt file line and distnce */
    size_t vector_iter = 3;
    input_error error = NO_ERROR, temp_error = NO_ERROR;
    bool was_digit = false;

    *vector = (double*) malloc(buffer_size * sizeof(double));

    while (*c != '\n' && *c != EOF)
    {
        char* vector_element = NULL;

        temp_error = get_data_member(&vector_element, data, &was_digit, c);
        if (temp_error)
        {
            error = temp_error;
        }

        if (*vector_element != '\0')
        {
            double temp_element = strtod(vector_element, NULL);
            if (temp_element == HUGE_VAL)
            {
                print_error(DOUBLE_NOT_REPRESENTABLE);
            }

            (*vector)[vector_iter++] = temp_element;
        }

        free(vector_element);

        if (vector_iter % buffer_size == 0)
        {
            *vector = realloc(*vector, 2 * vector_iter * sizeof(double));
        }
    }

    *vector = realloc(*vector, vector_iter * sizeof(double));

    (*vector)[0] = (double) vector_iter;

    return error;
}

size_t get_varray(double*** varray, FILE* data)
{
    int buffer_size = LARGE_BUFFER;

    /* c initialized as 'a' since letters are ignored by default */
    int c = 'a';

    size_t varray_iter = 0, prev_vector_size = 0;
    input_error error = NO_ERROR, temp_error = NO_ERROR;
    double line_counter = 0;

    *varray = (double**) malloc(buffer_size * sizeof(double*));

    while(c != EOF)  
    {
        ++line_counter;

        double* vector = NULL;
        temp_error = get_vector(&vector, data, &c);
        if (temp_error)
        {
            error = temp_error;
        }

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
            *varray = (double**) realloc(*varray,
                                        2 * varray_iter * sizeof(double*));
        }
    }

    *varray = (double**) realloc(*varray, varray_iter * sizeof(double*));

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

size_t read_from_file(char* adress, double*** varray)
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


// CALCULATIONS

double vector_length(double* vector)
{
    double sum = 0;

    for (size_t i = 3; i < vector[0]; ++i)
    {
        sum += pow((vector[i]), 2);
    }

    return sqrtf(sum);
}

double dot_product(double* vector_1, double* vector_2)
{
    if (vector_1[0] != vector_2[0])
    {
        print_error(DIFFERENT_VECTOR_DIMENSION);
    }

    double sum = 0;

    for (size_t i = 3; i < vector_1[0]; ++i)
    {
        sum += vector_1[i] * vector_2[i];
    }
    
    return sum;
}

double vector_mean(double* vector)
{
    double sum = 0;

    for (size_t i = 3; i < vector[0]; ++i)
    {
        sum += vector[i];
    }
    
    return (sum/(vector[0]-3));
}

double* centred_vector(double* vector)
{
    double* temp_vector = malloc(vector[0] * sizeof(double));

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

double correlation_distance(double* vector_1, double* vector_2)
{
    double* centred_vector_1 = centred_vector(vector_1);
    double* centred_vector_2 = centred_vector(vector_2);
    double denominator = vector_length(centred_vector_1)
                         * vector_length(centred_vector_2);
    
    if(denominator != 0)
    {
        double cdist = 1 - (dot_product(centred_vector_1, centred_vector_2)
                      / denominator);
        free(centred_vector_1);
        free(centred_vector_2);
        return cdist;
    }
    else
    {
        return -1;
    }
}

double euclidean_distance(double* vector_1, double* vector_2)
{
    double sum = 0;

    for (size_t i = 3; i < vector_1[0]; ++i)
    {
        sum += pow((vector_1[i] - vector_2[i]), 2);
    }

    return sqrtf(sum);
}

double city_block_distance(double* vector_1, double* vector_2)
{
    double sum = 0;

    for (size_t i = 3; i < vector_1[0]; ++i)
    {
        sum += fabs(vector_1[i] - vector_2[i]);
    }

    return sum;
}

int varray_sort(const void* a, const void* b)
{
    const double x = ((double*) *(double**)a)[2];
    const double y = ((double*) *(double**)b)[2];

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


// USER I/O

void print_error(int error)
{
    if (error == LETTER_AFTER_NUM)
    {
        printf("It looks like there are some typos in your data, "
               "which were ignored. The calculation will proceed, but "
               "please check your input to ensure the results are not flawed.\n\n"
               "Press [Enter] to continue...");
        getchar();
    }
    else if (error == DOUBLE_NOT_REPRESENTABLE)
    {
        printf("Some of values given are not representable by the double type "
               "Please check your input.\n"
               "The program will now terminate.\n\n"
               "Press [Enter] to continue...");
        getchar();
        exit(EXIT_FAILURE);
    }
    else if (error == DIFFERENT_VECTOR_DIMENSION)
    {
        printf("The vectors are not of the same dimension. "
               "Please check your input.\n"
               "The program will now terminate.\n\n"
               "Press [Enter] to continue...");
        getchar();
        exit(EXIT_FAILURE);
    }
    else if (error == EMPTY_FILE)
    {
        printf("Couldn't find any vectors in your input file. "
               "Please ensure it's filled properly.\n"
               "The program will now terminate.\n\n"
               "Press [Enter] to continue...");
        getchar();
        exit(EXIT_FAILURE);
    }
    else if (error == NO_FILE)
    {
        printf("Couldn't find the input file. "
               "Please ensure it's in the proper location.\n"
               "The program will now terminate.\n\n"
               "Press [Enter] to continue...");
        getchar();
        exit(EXIT_FAILURE);
    }
    if (error == CORRELATION_IMPOSSIBLE)
    {
        printf("Correlation distance couldn't be calculated for some of the "
               "given vectors (indicated by distance equal to -1 in output table");
        getchar();
    }
}

void print_neighbours(double** varray, long int k, bool vector_in_file)
{
    size_t a = 0;

    if (vector_in_file)
    {
        a = 1;
        ++k;
    }
    
    printf("\n");
    for (size_t i = a; i < k; ++i)
    {
        for (size_t j = 3; j < varray[i][0]; ++j)
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
    long int k = 0;

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
                         double*** varray, double* user_vector)
{
    input_error error = NO_ERROR;

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

            if ((*varray)[i][2] == -1)
            {
                error = CORRELATION_IMPOSSIBLE;
            }
        }
    }

    if (error)
    {
        print_error(error);
    }

    printf("\nDone\n");
}

double* get_user_vector(size_t vector_size)
{
    double* user_vector = NULL;

    printf("\nPlease give me your vector "
           "(separate elements with spaces or commas): ");

    int dummy = 'a';
    input_error error = get_vector(&user_vector, stdin, &dummy);
                
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

size_t get_file_vector_pos(double** varray, size_t varray_size)
{
    long int row_number = 0;

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


// MAIN

int main()
{
    printf("\nK Nearest Neighbours Calculator\n"
        "-------------------------------\n");

    double** varray = NULL;
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
        double* user_vector = get_user_vector(varray[0][0]);
        calculate_distances(distance_measure, varray_size, &varray, user_vector);
        free(user_vector);
    }

    qsort(varray, varray_size, sizeof(double*), varray_sort);

    long int k = 0; 
    if (vector_in_file)
    {
        k = get_k(varray_size - 1);;
    }
    else
    {
        k = get_k(varray_size);
    }

    print_neighbours(varray, k, vector_in_file);

    for (size_t i = 0; i < varray_size; ++i)
    {
        free(varray[i]);
    }
    
    free(varray);
    
    return 0;
}
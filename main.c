/*
TODO

Calculations:
- correlation distance

Pause and ponder:
- is there a better way than not sending was_digit from get_vector?
- check if returning nothing can be done boolean
  || (element_iter == 0 && info.c != '\n' && info.c != EOF)
  problem with reading empty vector (returns \0 anyway)
- is my error system the best option here?
- how to tell print_neighbours if vector was read?
- how to split into files?
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
    FLOAT_NOT_REPRESENTABLE,
    DIFFERENT_VECTOR_DIMENSION,
    EMPTY_FILE,
    NO_FILE
} read_error;

typedef enum distance_metric
{
    EUCLIDEAN,
    CITY_BLOCK,
    CORRELATION
} distance_metric;

typedef struct data_info
{
    size_t varray_size, vector_size;
    int c;
    read_error error;
} data_info;

void print_errors(int err)
{
    if (err == LETTER_AFTER_NUM)
    {
        printf("It looks like there are some typos in your data, "
               "which were ignored. The calculation will proceed, but "
               "please check your input to ensure the results are not flawed.\n\n"
               "Press [Enter] to continue...");
        while(getchar() != '\n');
    }
    else if (err == FLOAT_NOT_REPRESENTABLE)
    {
        printf("Some of values given are not representable by the float type "
               "Please check your input.\n"
               "The program will now terminate.\n\n"
               "Press [Enter] to continue...");
        while(getchar() != '\n');
        exit(EXIT_FAILURE);
    }
    else if (err == DIFFERENT_VECTOR_DIMENSION)
    {
        printf("The vectors are not of the same dimension. "
               "Please check your input.\n"
               "The program will now terminate.\n\n"
               "Press [Enter] to continue...");
        while(getchar() != '\n');
        exit(EXIT_FAILURE);
    }
    else if (err == EMPTY_FILE)
    {
        printf("Couldn't find any vectors in your input file. "
               "Please ensure it's filled properly.\n"
               "The program will now terminate.\n\n"
               "Press [Enter] to continue...");
        while(getchar() != '\n');
        exit(EXIT_FAILURE);
    }
    else if (err == EMPTY_FILE)
    {
        printf("Couldn't find the input file. "
               "Please ensure it's in the proper location.\n"
               "The program will now terminate.\n\n"
               "Press [Enter] to continue...");
        while(getchar() != '\n');
        exit(EXIT_FAILURE);
    }
}


// INPUT PARSER

long int getlint(FILE* stream)
{
    char* buffer;
    size_t buf_size = 32;
    buffer = (char*) malloc(buf_size * sizeof(char));

    getline(&buffer, &buf_size, stream);
    long int num = strtol(buffer, NULL, 0);

    free(buffer);

    return num;
}

data_info get_data_member(char** vector_element, FILE* data, bool *was_digit)
{
    int buffer_size = 128;

    data_info info = {sizeof(float*), sizeof(float), 'a', NO_ERROR};

    /* initialized as 'a' since letters are ignored by default */
    int prev = 'a', prev_2 = 'a', prev_3 = 'a';

    size_t element_iter = 0;
    bool is_exp = false;

    *vector_element = (char*) malloc(buffer_size * sizeof(char));

    while (info.c != ' ' && info.c != ',' && info.c != '\n' && info.c != EOF)
    {
        prev_3 = prev_2;
        prev_2 = prev;
        prev = info.c;
        info.c = getc(data);

        //printf("%c ", info.c);

        if ((isdigit((char) prev_2) && (prev == '.') && isdigit((char) info.c))
            || (isdigit((char) prev_2) && (prev == 'e')
            && (isdigit((char) info.c) || info.c == '-') && !is_exp)
            || ((!isdigit((char) prev_2) && prev_2 != 'e') && prev == '-'
            && isdigit((char) info.c)))
        {
            //printf(" Case 1 happened ");

            (*vector_element)[element_iter++] = (char) prev;
            (*vector_element)[element_iter++] = (char) info.c;

            if (!is_exp)
            {
                is_exp = true;
            }
        }
        else if (!isdigit((char) info.c) && info.c != '-' && info.c != '.'
                 && info.c != 'e' && info.c != ' ' && info.c != ','
                 && info.c != '\n' && info.c != EOF && *was_digit)
        {
            //printf(" Case 2 happened ");
            info.error = LETTER_AFTER_NUM;
        }
        else if (isdigit((char) info.c))
        {
            //printf(" Case 3 happened ");
            (*vector_element)[element_iter++] = (char) info.c;
            *was_digit = true;
        }

        if (element_iter != 0 && element_iter % buffer_size == 0)
        {
            *vector_element = realloc(*vector_element,
                                    (element_iter + buffer_size)
                                    * sizeof(char));
        }
    }

    //printf("\n");

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

    /* first two members are reserved for the data.txt file line and distnce */
    size_t vector_iter = 2;
    read_error local_error = NO_ERROR;
    bool was_digit = false;

    *vector = (float*) malloc(buffer_size * sizeof(float));

    while (info.c != '\n' && info.c != EOF)
    {
        char* vector_element;
        float temp_element;

        info = get_data_member(&vector_element, data, &was_digit);
        if (info.error && !local_error)
        {
            local_error = info.error;
        }

        //printf("  %s  \n", vector_element);

        if (*vector_element != '\0')
        {
            if ((temp_element = strtof(vector_element, NULL)) == HUGE_VALF)
            {
                local_error = FLOAT_NOT_REPRESENTABLE;
                print_errors(local_error);
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

    info.vector_size = vector_iter;
    info.error = local_error;

    return info;
}

data_info get_varray(float*** varray, FILE* data)
{
    int buffer_size = 128;

    /* c initialized as 'a' since letters are ignored by default */
    data_info info = {sizeof(float*), sizeof(float), 'a', NO_ERROR};

    size_t varray_iter = 0, prev_vector_size;
    read_error local_error = NO_ERROR;
    float line_counter = 0;

    *varray = (float**) malloc(buffer_size * sizeof(float*));

    while(info.c != EOF)  
    {
        ++line_counter;

        float* vector;
        info = get_vector(&vector, data);
        if (info.error && !local_error)
        {
            local_error = info.error;
        }

        //printf("%zu", varray_iter);
        //printf("%zu ", info.vector_size);
        //printf("%zu %zu ", info.vector_size, prev_vector_size);
        if (varray_iter != 0 && info.vector_size != prev_vector_size
            && info.vector_size != 2)
        {
            //printf("executed");
            local_error = DIFFERENT_VECTOR_DIMENSION;
        }

        if (info.vector_size != 2)
        {
            prev_vector_size = info.vector_size;
            vector[0] = line_counter;
            (*varray)[varray_iter++] = vector;
        }

        if (varray_iter != 0 && varray_iter % buffer_size == 0)
        {
            *varray = (float**) realloc(*varray,
                                        2 * varray_iter * sizeof(float*));
        }
    }

    *varray = (float**) realloc(*varray, varray_iter * sizeof(float*));

    info.varray_size = varray_iter;
    info.error = local_error;
    
    return info;
}


// CALCULATIONS

float euclidean_distance(size_t varray_pos, size_t vector_size, float** varray,
                         float* user_vector)
{
    float sum = 0;

    for (size_t j = 2; j < vector_size; ++j)
    {
        sum += pow((varray[varray_pos][j] - user_vector[j]), 2);
    }

    return sqrtf(sum);
}

float city_block_distance(size_t varray_pos, size_t vector_size, float** varray,
                          float* user_vector)
{
    float sum = 0;

    for (size_t j = 2; j < vector_size; ++j)
    {
        sum += fabsf(varray[varray_pos][j] - user_vector[j]);
    }

    return sum;
}

int varray_sort(const void* a, const void* b)
{
    const float x = ((float*) *(float**)a)[1];
    const float y = ((float*) *(float**)b)[1];
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
        for (size_t j = 2; j < vector_size; ++j)
        {
            printf("%f ", varray[i][j]);
        }
        printf("  Distance: %f  At file line: %.0f\n", varray[i][1], varray[i][0]);
    }
    printf("\n");
}

long int get_k(data_info file_info)
{
    long int k;

    while(1)
    {
        printf("\nHow many nearest neighbours would you like to print? ");
        k = getlint(stdin);
        
        if (k)
        {
            if (k <= file_info.varray_size)
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

void calculate_distances(distance_metric metric, data_info file_info,
                         float*** varray, float* user_vector)
{
    for (size_t i = 0; i < file_info.varray_size; ++i)
    {   
        if (metric == EUCLIDEAN)
        {
            (*varray)[i][1] = euclidean_distance(i, file_info.vector_size, *varray,
                                            user_vector);
        }
        else if (metric == CITY_BLOCK)
        {
            (*varray)[i][1] = city_block_distance(i, file_info.vector_size, *varray,
                                                user_vector);
        }
        
    }
}

float* get_user_vector(data_info file_info)
{
    float* user_vector;

    printf("\nPlease give me your vector "
           "(separate elements with spaces or commas): ");

    data_info user_info = get_vector(&user_vector, stdin);
                
    if (user_info.vector_size != file_info.vector_size)
    {
        user_info.error = DIFFERENT_VECTOR_DIMENSION;
    }

    if (user_info.error)
    {
        printf("\nFollowing problem was found in your input vector:\n\n");
        print_errors(user_info.error);
        printf("\n");
    }

    return user_vector;
}

size_t get_file_vector_pos(float** varray, data_info file_info)
{
    long int row_number;

    while(1)
    {
        printf("\nPlease specify the file row with the desired vector: ");
        row_number = getlint(stdin);

        if (row_number)
        {
            for (size_t i = 0; i < file_info.varray_size; ++i)
            {
                if (varray[i][0] == row_number)
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

distance_metric choose_metric()
{
    while (1)
    {
        printf("\nWhich metric would you like to use in your calculations?\n\n"
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
                printf("\nMetric was changed to euclidean.\n\n");
                return EUCLIDEAN;
            case 2:
                printf("\nMetric was changed to city-block.\n\n");
                return CITY_BLOCK;
            case 3:
                printf("\nMetric was changed to correlation.\n\n");
                return CORRELATION;
            default:
                printf("\nSeems like your input is incorrect. "
                       "Please try one more time.\n\n");
                break;
        }
    }
}

data_info read_from_file(char* adress, float*** varray)
{
    printf("\nReading input file...\n");

    FILE* data = fopen(adress, "r");
    if (data == NULL)
    {
        printf("Following problem was found with your input data file:\n\n");
        print_errors(NO_FILE);
        printf("\n");
    }

    data_info file_info = get_varray(varray, data);
    if (file_info.varray_size == 0)
    {
        file_info.error = EMPTY_FILE;
    }

    fclose(data);
    printf("\nDone.\n");

    if (file_info.error)
    {
        printf("Following problem was found in your input data file:\n\n");
        print_errors(file_info.error);
        printf("\n");
    }
    
    return file_info;
}

int main()
{
    printf("\nK-Nearest-Neighbours Calculator\n"
        "-------------------------------\n");

    float** varray;
    data_info file_info = read_from_file("data//data.txt", &varray);    

    distance_metric metric = choose_metric();
    
    bool vector_in_file = user_vector_in_file();
    if (vector_in_file)
    {
        size_t user_vector_pos = get_file_vector_pos(varray, file_info);
        calculate_distances(metric, file_info, &varray, varray[user_vector_pos]);
    }
    else
    {
        float* user_vector = get_user_vector(file_info);
        calculate_distances(metric, file_info, &varray, user_vector);
        free(user_vector);
    }

    qsort(varray, file_info.varray_size, sizeof(float*), varray_sort);

    long int k = get_k(file_info);

    print_neighbours(varray, file_info.vector_size, k, vector_in_file);

    for (size_t i = 0; i < file_info.varray_size; ++i)
    {
        free(varray[i]);
    }
    
    free(varray);
    
    return 0;
}
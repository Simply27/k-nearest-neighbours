/*
- reading file input
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

void get_data(char** data_member, FILE** data)
{
    *data_member = (char*) malloc(128 * sizeof(char));
    if (*data_member)
    {
        size_t n = 0;
        int c;

        while ((c = fgetc(*data)) != ' ')
        {
            if (isdigit((char) c))
            {
                (*data_member)[n++] = (char) c;
            }
        }

        (*data_member)[n] = '\0';
    }
}

void file_to_vectors()
{
    FILE *data = fopen("data.txt", "r");
    if (data == NULL)
    {
        printf("No data in file!");
    }

    char *data_member = NULL;

    get_data(&data_member, &data);

    printf("%s", data_member);
}

int main()
{
    file_to_vectors();
    return 0;
}
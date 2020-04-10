/*
- reading file input
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void file_to_vectors()
{
    FILE *data = fopen("data.txt", "r");

    int i = 0;
    char c, *data_member;

    while(c != ' ')
    {
        c = fgetc(data);
        data_member = (char *) realloc(data_member, sizeof(char));
        data_member[i] = c;
        ++i;
    }

    printf("%s", data_member);

    free(data_member);
    fclose(data);

    /*
    char *file_line = NULL, *data_member = NULL;
    size_t file_line_len, data_member_len;

    while(getdelim(&file_line, &data_member_len, '\n', data) != -1)
    {
        while()
        {
            printf("%s ", data_member);
        }
        printf("\n");
    }
    */
}

int main()
{
    file_to_vectors();
    return 0;
}
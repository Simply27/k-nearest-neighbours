/*
- reading file input
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct parser_params
{
    char *start;
    char *finish;
    char delim;
}parser_params;

int parser(char *line, char del, char *newstr)
{
    if(line)
    {
        parser_params params = {.delim = del};
        params.start = line;
        params.finish = params.start;
        while (*params.finish != params.delim || *params.finish != '\0')
        {
            ++params.finish;
        }

        newstr = malloc(params.finish - params.start);
        
        while(params.start != params.finish)
        {
            *newstr = *params.start;
            ++newstr;
            ++params.start;
        }
    }
    else
    {
        fprintf(stderr, "line is a null pointer!");
        return(-1);
    }
    
}

void file_to_vectors()
{
    FILE *data = fopen("data.txt", "r");

    char *file_line = NULL, *data_member = NULL;
    size_t file_line_len, data_member_len;

    getline(&file_line, &data_member_len, data);
    parser(file_line, ' ', data_member);
    printf("%s", file_line);
}

int main()
{
    file_to_vectors();
    return 0;
}
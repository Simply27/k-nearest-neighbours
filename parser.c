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
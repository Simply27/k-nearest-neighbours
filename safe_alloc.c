void* safe_malloc(size_t size, int line)
{
    void* ptr = malloc(size);

    if (ptr == NULL)
    {
        FILE* err_log = fopen("err_log.txt", "w");
        fprintf(err_log, "Failed memory allocation: %zu bytes at line %d.\n",
                size, line);
        fclose(err_log);
        exit(1);
    }

    return ptr;
}

void* safe_realloc(void* ptr, size_t size, int line)
{
    void* new_ptr = realloc(ptr, size);

    if (new_ptr == NULL)
    {
        FILE* err_log = fopen("err_log.txt", "w");
        fprintf(err_log, "Failed memory reallocation: %zu bytes at line %d.\n",
                size, line);
        fclose(err_log);
        exit(1);
    }

    return new_ptr;
}

void safe_fopen()
{

}
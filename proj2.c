#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool is_number(char *string) // function to check if string is numeric
{
    for (int i = 0; i < (int)strlen(string); i++)
        if (string[i] < '0' || string[i] > '9')
        {
            fprintf(stderr, "Argunment has to be numeric or > 0 ( %s )\n", string);
            return false;
        }
    return true;
}

bool check_args(int argc, char **argv) // argument checking function
{
    if (argc != 5) // if argument count does not equal 5
    {
        fprintf(stderr, "Argument count does not equal parameters of program ( must be 4 <=> was %d ) \n", argc - 1);
        return false;
    }
    for (int i = 1; i < argc; i++) // check if every argument is numeric
        if (!is_number(argv[i]))
            return false;
    return true;
}

int main(int argc, char **argv)
{
    if (!check_args(argc, argv))
        return 1;

    return 0;
}
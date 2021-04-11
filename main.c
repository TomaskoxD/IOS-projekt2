#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool is_number(char *string)
{
    for (int i = 0; i < (int)strlen(string); i++)
    {
        if (string[i] <= '0' || string[i] >= '9')
            return false;
    }
    return true;
}

bool check_args(int argc, char **argv)
{
    if (argc != 5)
    {
        return false;
    }
    for (int i = 1; i < argc; i++)
    {
        if (!is_number(argv[i]))
            return false;
    }

    return true;
}

int main(int argc, char **argv)
{
    if (!check_args(argc, argv))
        return -1;
    printf("preslo");
    return 0;
}
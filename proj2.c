#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>
typedef struct // struct for integer parameters
{
    int NE;
    int NR;
    int TE;
    int TR;
} params;

bool is_number(char *string) // function to check if string is numeric
{
    for (int i = 0; i < (int)strlen(string); i++)
        if (string[i] < '0' || string[i] > '9')
        {
            fprintf(stderr, "ERROR : Argument has to be numeric or > 0 ( %s )\n", string);
            return false;
        }
    return true;
}

bool is_in_range(int arg, char *string) // function to check if string is in range of function parameters
{
    int value = atoi(string);
    if (arg == 2)
    {
        if (value >= 20)
        {
            fprintf(stderr, "ERROR : Argument at %d. position does not equal parameters of program ( must be <20 <=> was %d ) \n", arg, value);
            return false;
        }
    }
    else
    {
        if (value >= 1000)
        {
            fprintf(stderr, "ERROR : Argument at %d. position does not equal parameters of program ( must be <1000 <=> was %d ) \n", arg, value);
            return false;
        }
    }
    return true;
}

bool check_args(int argc, char **argv) // argument checking function
{
    if (argc != 5) // if argument count does not equal 5
    {
        fprintf(stderr, "ERROR : Argument count does not equal parameters of program ( must be 4 <=> was %d ) \n", argc - 1);
        return false;
    }
    for (int i = 1; i < argc; i++) // check if every argument is numeric
        if (!is_number(argv[i]) || !is_in_range(i, argv[i]))
            return false;
    return true;
}

void fill_struct(params *params, char **argv) // function to fill up struct with integer parameters
{
    params->NE = atoi(argv[1]);
    params->NR = atoi(argv[2]);
    params->TE = atoi(argv[3]);
    params->TR = atoi(argv[4]);
    return;
}

#define free_shared_variables             \
    if (true)                             \
    {                                     \
        munmap(needHelp, sizeof(int));    \
        munmap(elfID, sizeof(int));       \
        munmap(rID, sizeof(int));         \
        munmap(actionCount, sizeof(int)); \
    }

int main(int argc, char **argv)
{

    setbuf(stderr, NULL);
    if (!check_args(argc, argv))
        return 1;
    params params;
    fill_struct(&params, argv);

    FILE *fpointer;
    fpointer = fopen("proj2.out", "w");
    if (fpointer == NULL)
    {
        fprintf(stderr, "ERROR : Opening file unsuccessfull\n");
        return 1;
    }
    setbuf(fpointer, NULL);

    //Initialization of shared memory
    int *needHelp = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int *elfID = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int *rID = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int *actionCount = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *needHelp = 0;
    *elfID = 0;
    *rID = 0;
    *actionCount = 1;

    free_shared_variables;

    fclose(fpointer);
    printf("Done!\n");
    return 0;
}
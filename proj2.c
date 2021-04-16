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

#define free_shared_variables               \
    if (true)                               \
    {                                       \
        munmap(needHelp, sizeof(int));      \
        munmap(elfID, sizeof(int));         \
        munmap(rID, sizeof(int));           \
        munmap(actionCount, sizeof(int));   \
        munmap(hitchedCount, sizeof(int));  \
        munmap(returnedCount, sizeof(int)); \
        munmap(help1, sizeof(int));         \
        munmap(help2, sizeof(int));         \
        munmap(help3, sizeof(int));         \
        munmap(queue, sizeof(int));         \
    }
#define unmap_and_destroy_sems                      \
    if (true)                                       \
    {                                               \
        munmap(sem_drain, sizeof(sem_t));           \
        sem_destroy(sem_drain);                     \
        munmap(sem_wake_santa, sizeof(sem_t));      \
        sem_destroy(sem_wake_santa);                \
        munmap(sem_helped_elfes, sizeof(sem_t));    \
        sem_destroy(sem_helped_elfes);              \
        munmap(sem_hitch, sizeof(sem_t));           \
        sem_destroy(sem_hitch);                     \
        munmap(sem_last_hitched, sizeof(sem_t));    \
        sem_destroy(sem_last_hitched);              \
        munmap(sem_last_returned, sizeof(sem_t));   \
        sem_destroy(sem_last_returned);             \
        munmap(sem_workshop_closed, sizeof(sem_t)); \
        sem_destroy(sem_workshop_closed);           \
    }
int main(int argc, char **argv)
{
    time_t randtime;
    srand((unsigned)time(&randtime));

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
    int *hitchedCount = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int *returnedCount = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int *help1 = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int *help2 = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int *help3 = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int *queue = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *needHelp = 0;
    *elfID = 0;
    *rID = 0;
    *actionCount = 1;
    *hitchedCount = 0;
    *returnedCount = 0;
    *queue = 0;

    sem_t *sem_drain = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *sem_wake_santa = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *sem_helped_elfes = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *sem_hitch = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *sem_last_hitched = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *sem_last_returned = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *sem_workshop_closed = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    sem_init(sem_drain, 1, 1);
    sem_init(sem_wake_santa, 1, 0);
    sem_init(sem_helped_elfes, 1, 0);
    sem_init(sem_hitch, 1, 0);
    sem_init(sem_last_hitched, 1, 0);
    sem_init(sem_last_returned, 1, 0);
    sem_init(sem_workshop_closed, 1, 0);

    int mainP;
    int santaP;
    int elfP;
    int sobP;

    //Fork main procesu pre vytvorenie subprocessu
    if ((mainP = fork()) < 0)
    {
        free_shared_variables;

        munmap(sem_drain, sizeof(sem_t));
        sem_destroy(sem_drain);

        fprintf(stderr, "%s\n", "Error: fork main process");
        return 1;
    }

    if (mainP == 0) //Subprocess pre generovanie pasazierov
    {
        if ((santaP = fork()) < 0)
        {
            free_shared_variables;
            unmap_and_destroy_sems;
            fprintf(stderr, "%s\n", "Error: fork subprocess for Santa");
            exit(1);
        }

        if (santaP == 0)
        {
            fprintf(fpointer, "%d %s%s\n", *actionCount, ": Santa", ": going to sleep");
            *actionCount = *actionCount + 1;

            sem_wait(sem_wake_santa);
            fprintf(fpointer, "%d %s%s\n", *actionCount, ": Santa", ": helping elfes");
            *actionCount = *actionCount + 1;
            fprintf(fpointer, "%d %s %d %s\n", *actionCount, ": Elf ", *help1, ": get help");
            *actionCount = *actionCount + 1;
            fprintf(fpointer, "%d %s %d %s\n", *actionCount, ": Elf ", *help2, ": get help");
            *actionCount = *actionCount + 1;
            fprintf(fpointer, "%d %s %d %s\n", *actionCount, ": Elf ", *help3, ": get help");
            *actionCount = *actionCount + 1;
            fprintf(fpointer, "%d %s%s\n", *actionCount, ": Santa", ": going to sleep");
            *actionCount = *actionCount + 1;
            for (int i = 0; i < params.NR; i++)
                sem_post(sem_helped_elfes);

            sem_wait(sem_last_returned);
            fprintf(fpointer, "%d %s%s\n", *actionCount, ": Santa", ": closing workshop");
            *actionCount = *actionCount + 1;
            for (int i = 0; i < params.NE; i++)
                sem_post(sem_workshop_closed);
            for (int i = 0; i < params.NR; i++)
                sem_post(sem_hitch);

            sem_wait(sem_last_hitched);
            fprintf(fpointer, "%d %s%s\n", *actionCount, ": Santa", ": Christmas started");
            *actionCount = *actionCount + 1;
            exit(0);
        }
        for (int k = 0; k < params.NE; k++)
        {
            if ((elfP = fork()) < 0)
            {
                free_shared_variables;
                unmap_and_destroy_sems;
                fprintf(stderr, "%s\n", "Error: fork subprocess for elfs");
                exit(1);
            }

            if (elfP == 0)
            {
                *elfID = *elfID + 1;
                int ELFid = *elfID;

                sem_wait(sem_drain);
                fprintf(fpointer, "%d %s %d %s\n", *actionCount, ": Elf ", ELFid, ": started");
                *actionCount = *actionCount + 1;
                sem_post(sem_drain);

                usleep(rand() % params.TE);

                sem_wait(sem_drain);
                fprintf(fpointer, "%d %s %d %s\n", *actionCount, ": Elf ", ELFid, ": need help");
                *actionCount = *actionCount + 1;
                *needHelp = *needHelp + 1;
                if (*needHelp == 1)
                    *help1 = ELFid;
                if (*needHelp == 2)
                    *help2 = ELFid;
                if (*needHelp == 3)
                {
                    *help3 = ELFid;

                    fprintf(fpointer, "\t\tSemaforujem na santu ze cakaju traja skreckovia\n");
                    sem_post(sem_wake_santa);
                }
                sem_post(sem_drain);

                sem_wait(sem_helped_elfes);
                sem_wait(sem_drain);

                sem_post(sem_drain);

                sem_wait(sem_workshop_closed);
                sem_wait(sem_drain);
                fprintf(fpointer, "%d %s %d %s\n", *actionCount, ": Elf ", ELFid, ": taking holidays");
                *actionCount = *actionCount + 1;
                sem_post(sem_drain);

                exit(0);
            }
        }

        for (int k = 0; k < params.NR; k++)
        {
            if ((sobP = fork()) < 0)
            {
                free_shared_variables;
                unmap_and_destroy_sems;
                fprintf(stderr, "%s\n", "Error: fork subprocess for sobs");
                exit(1);
            }

            if (sobP == 0)
            {
                *rID = *rID + 1;
                int Rid = *rID;

                sem_wait(sem_drain);
                fprintf(fpointer, "%d %s %d %s\n", *actionCount, ": RD ", Rid, ": rstarted");
                *actionCount = *actionCount + 1;
                sem_post(sem_drain);

                usleep(((rand() % params.TR) + params.TR) / params.TR);

                sem_wait(sem_drain);
                *returnedCount = *returnedCount + 1;
                fprintf(fpointer, "%d %s %d %s\n", *actionCount, ": RD ", Rid, ": return home");
                *actionCount = *actionCount + 1;
                if (*returnedCount == params.NR)
                {
                    fprintf(fpointer, "\t\tSemaforujem na santu ze dosiel posledny sob\n");
                    sem_post(sem_last_returned);
                }
                sem_post(sem_drain);

                sem_wait(sem_hitch);

                sem_wait(sem_drain);
                *hitchedCount = *hitchedCount + 1;
                fprintf(fpointer, "%d %s %d %s\n", *actionCount, ": RD ", Rid, ": get hitched");
                *actionCount = *actionCount + 1;
                if (*hitchedCount == params.NR)
                {
                    fprintf(fpointer, "\t\tSemaforujem na ukoncenie santu \n");
                    sem_post(sem_last_hitched);
                }
                sem_post(sem_drain);
                exit(0);
            }
        }

        exit(0);
    }

    free_shared_variables;
    unmap_and_destroy_sems;
    fclose(fpointer);
    printf("Done!\n");
    return 0;
}
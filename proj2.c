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
        if (value >= 20 || value == 0)
        {
            fprintf(stderr, "ERROR : Argument at %d. position does not equal parameters of program ( must be 0<x<20 <=> was %d ) \n", arg, value);
            return false;
        }
    }
    else if (arg == 3 || arg == 4)
    {
        if (value > 1000)
        {
            fprintf(stderr, "ERROR : Argument at %d. position does not equal parameters of program ( must be 0<=x<=1000 <=> was %d ) \n", arg, value);
            return false;
        }
    }
    else
    {
        if (value >= 1000 || value == 0)
        {
            fprintf(stderr, "ERROR : Argument at %d. position does not equal parameters of program ( must be 0<x<1000 <==> was %d ) \n", arg, value);
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
    if (atoi(argv[3]) != 0)
        params->TE = atoi(argv[3]);
    else
        params->TE = 1;
    if (atoi(argv[4]) != 0)
        params->TR = atoi(argv[4]);
    else
        params->TR = 1;
    return;
}

// macro for easy free-ing shared memory
#define free_shared_variables               \
    do                                      \
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
    } while (0)

// macro for unmapping and destroying semaphores
#define unmap_and_destroy_sems                      \
    do                                              \
    {                                               \
        sem_destroy(sem_drain);                     \
        munmap(sem_drain, sizeof(sem_t));           \
        sem_destroy(sem_wake_santa);                \
        munmap(sem_wake_santa, sizeof(sem_t));      \
        sem_destroy(sem_helped_elfes);              \
        munmap(sem_helped_elfes, sizeof(sem_t));    \
        sem_destroy(sem_hitch);                     \
        munmap(sem_hitch, sizeof(sem_t));           \
        sem_destroy(sem_last_hitched);              \
        munmap(sem_last_hitched, sizeof(sem_t));    \
        sem_destroy(sem_last_returned);             \
        munmap(sem_last_returned, sizeof(sem_t));   \
        sem_destroy(sem_workshop_closed);           \
        munmap(sem_workshop_closed, sizeof(sem_t)); \
        sem_destroy(sem_main_wait);                 \
        munmap(sem_main_wait, sizeof(sem_t));       \
    } while (0)

int main(int argc, char **argv)
{
    time_t randtime;
    srand((unsigned)time(&randtime)); // creating random time seed

    setbuf(stderr, NULL);
    if (!check_args(argc, argv)) // check if all arguments are correct 
        return 1;
    params params; // declare struct 
    fill_struct(&params, argv); // filling up struct with function arguments

    FILE *fpointer;
    fpointer = fopen("proj2.out", "w"); // open file 'proj2.out' for writing out log data
    if (fpointer == NULL)               // if file opening was unsuccessfull
    {
        fprintf(stderr, "ERROR : Opening file unsuccessfull\n");
        return 1;
    }
    setbuf(fpointer, NULL);

    // Declaration of shared memory
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
    // Initialization of shared memory
    *needHelp = 0;
    *elfID = 0;
    *rID = 0;
    *actionCount = 1;
    *hitchedCount = 0;
    *returnedCount = 0;
    *queue = 0;
    // Declaration of semaphores
    sem_t *sem_drain = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *sem_wake_santa = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *sem_helped_elfes = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *sem_hitch = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *sem_last_hitched = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *sem_last_returned = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *sem_workshop_closed = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *sem_main_wait = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // Initialization of semaphores
    sem_init(sem_drain, 1, 1); // mutex semaphore
    sem_init(sem_wake_santa, 1, 0);
    sem_init(sem_helped_elfes, 1, 0);
    sem_init(sem_hitch, 1, 0);
    sem_init(sem_last_hitched, 1, 0);
    sem_init(sem_last_returned, 1, 0);
    sem_init(sem_workshop_closed, 1, 0);
    sem_init(sem_main_wait, 1, 0);

    int mainP;
    int santaP;
    int elfP;
    int sobP;

    //Fork of main process to create sub-process
    if ((mainP = fork()) < 0) // if forking was unsuccessfull
    {
        free_shared_variables;
        unmap_and_destroy_sems;
        fprintf(stderr, "%s\n", "Error: Forking of main process failed");
        fclose(fpointer);
        exit(1);
    }
    if (mainP == 0) //Subprocess for generating other sub-processes
    {
        if ((santaP = fork()) < 0)
        {
            free_shared_variables;
            unmap_and_destroy_sems;
            fprintf(stderr, "%s\n", "Error: Fork for subprocess Santa failed");
            fclose(fpointer);
            exit(1);
        }
        if (santaP == 0)
        {
            sem_wait(sem_drain);
            fprintf(fpointer, "%d%s%s\n", *actionCount, ": Santa", ": going to sleep");
            (*actionCount)++;
            sem_post(sem_drain);
            if (params.NE >= 3) // if there is less than 3 elves in total, Santa is waiting till all rein.. return
            {
                sem_wait(sem_wake_santa); // waiting for 3rd elf to wake santa up
                sem_wait(sem_drain);
                fprintf(fpointer, "%d%s%s\n", *actionCount, ": Santa", ": helping elves");
                (*actionCount)++;
                fprintf(fpointer, "%d%s %d%s\n", *actionCount, ": Elf ", *help1, ": get help");
                (*actionCount)++;
                fprintf(fpointer, "%d%s %d%s\n", *actionCount, ": Elf ", *help2, ": get help");
                (*actionCount)++;
                fprintf(fpointer, "%d%s %d%s\n", *actionCount, ": Elf ", *help3, ": get help");
                (*actionCount)++;
                fprintf(fpointer, "%d%s%s\n", *actionCount, ": Santa", ": going to sleep");
                (*actionCount)++;
                sem_post(sem_drain);
            }
            for (int i = 0; i < params.NE; i++) // notify all elfes that Santa has helped them
                sem_post(sem_helped_elfes);

            sem_wait(sem_last_returned);
            sem_wait(sem_drain);
            fprintf(fpointer, "%d%s%s\n", *actionCount, ": Santa", ": closing workshop");
            (*actionCount)++;
            sem_post(sem_drain);

            for (int i = 0; i < params.NE; i++)
                sem_post(sem_workshop_closed);
            for (int i = 0; i < params.NR; i++) // let all get hitched
                sem_post(sem_hitch);

            sem_wait(sem_last_hitched); // wait for last one to get hitched
            sem_wait(sem_drain);
            fprintf(fpointer, "%d%s%s\n", *actionCount, ": Santa", ": Christmas started");
            (*actionCount)++;
            sem_post(sem_drain);

            sem_post(sem_main_wait);

            exit(0);
        }

        for (int i = 0; i < params.NE; i++)
        {
            if ((elfP = fork()) < 0)
            {
                free_shared_variables;
                unmap_and_destroy_sems;
                fprintf(stderr, "%s\n", "Error: fork subprocess elfs failed");
                fclose(fpointer);
                exit(1);
            }
            if (elfP == 0)
            {
                sem_wait(sem_drain);
                *elfID = *elfID + 1;
                int ELFid = *elfID;
                sem_post(sem_drain);

		srand((unsigned)time(&randtime) * ELFid); // creating random time seed for each process

                sem_wait(sem_drain);
                fprintf(fpointer, "%d%s %d%s\n", *actionCount, ": Elf ", ELFid, ": started");
                (*actionCount)++;
                sem_post(sem_drain);

                usleep((rand() % params.TE) * 1000); // random sleep for each elf process

                sem_wait(sem_drain);
                fprintf(fpointer, "%d%s %d%s\n", *actionCount, ": Elf ", ELFid, ": need help");
                (*actionCount)++;
                (*needHelp)++;
                if (*needHelp == 1) // save IDs for elfes in queue
                    *help1 = ELFid;
                if (*needHelp == 2)
                    *help2 = ELFid;
                if (*needHelp == 3)
                {
                    *help3 = ELFid;
                    sem_post(sem_wake_santa);
                }
                sem_post(sem_drain);

                sem_wait(sem_helped_elfes);

                if (params.NE >= 3) // if there is less than 3 elves in total, no elf will get help from Santa
                    if (*help1 == ELFid || *help2 == ELFid || *help3 == ELFid)
                    {
                        usleep((rand() % params.TE) * 1000); // random sleep for required elf process

                        sem_wait(sem_drain);
                        fprintf(fpointer, "%d%s %d%s\n", *actionCount, ": Elf ", ELFid, ": need help");
                        (*actionCount)++;
                        sem_post(sem_drain);
                    }

                sem_wait(sem_workshop_closed);
                sem_wait(sem_drain);
                fprintf(fpointer, "%d%s %d%s\n", *actionCount, ": Elf ", ELFid, ": taking holidays");
                (*actionCount)++;
                sem_post(sem_drain);

                sem_post(sem_main_wait);

                exit(0);
            }
        }

        for (int i = 0; i < params.NR; i++)
        {
            if ((sobP = fork()) < 0)
            {
                free_shared_variables;
                unmap_and_destroy_sems;
                fprintf(stderr, "%s\n", "Error: fork for subprocess reindeer failed");
                fclose(fpointer);
                exit(1);
            }
            if (sobP == 0)
            {
                sem_wait(sem_drain);
                *rID = *rID + 1;
                int Rid = *rID;
                sem_post(sem_drain);

		srand((unsigned)time(&randtime) * Rid); // creating random time seed for each process

                sem_wait(sem_drain);
                fprintf(fpointer, "%d%s %d%s\n", *actionCount, ": RD ", Rid, ": rstarted");
                (*actionCount)++;
                sem_post(sem_drain);

                usleep((((rand() % params.TR) + params.TR) / 2) * 1000); // random sleep for each process

                sem_wait(sem_drain);
                (*returnedCount)++;
                fprintf(fpointer, "%d%s %d%s\n", *actionCount, ": RD ", Rid, ": return home");
                (*actionCount)++;
                if (*returnedCount == params.NR) // if count of returned is equal to count of all
                    sem_post(sem_last_returned);
                sem_post(sem_drain);

                sem_wait(sem_hitch);
                sem_wait(sem_drain);
                (*hitchedCount)++;
                fprintf(fpointer, "%d%s %d%s\n", *actionCount, ": RD ", Rid, ": get hitched");
                (*actionCount)++;
                if (*hitchedCount == params.NR) // if count of hitched is equal to count of all
                    sem_post(sem_last_hitched);
                sem_post(sem_drain);

                sem_post(sem_main_wait);

                exit(0);
            }
        }
        exit(0);
    }

    // wait for all processes to finish
    for (int i = 0; i < params.NE + params.NR + 1; i++)
        sem_wait(sem_main_wait);

    free_shared_variables;
    unmap_and_destroy_sems;
    fclose(fpointer);
    exit(0);
}

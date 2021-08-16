#ifndef INIT_ARGS_H
#define INIT_ARGS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/ipc.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/stat.h>

typedef char* string;

#define WRONG_NUMBER_ARGS  "You have entered wrong number of arguments!\n"
#define WRONG_ARG          "You have entered wrong argument!\n"
#define ERROR_WITH_FILE    "Error with opening of the file for output!\n"
#define ERR_SHARED_MEMORY  "Error with initialization of shared memory!\n"
#define ERR_INIT_SEM       "Error with initialization of semaphore!\n"
#define ERR_CREATE_CHILD   "Creation of a child process was unsuccessful!\n"

#define HELP_FROM_SANTA    "/xshcha00_help_from_santa"
#define WAIT_WRITE_TO_FILE "/xshcha00_wait_write_to_file"
#define SANTA_WAKE_UP      "/xshcha00_santa_wake_up"
#define DEER_WAIT          "/xshcha00_deer_wait"
#define WAIT_ALL_DEERS     "/xshcha00_wait_all_deers"
#define CHANGE_VALUE       "/xshcha00_change_value"
#define ELVES_GET_HELP     "/xshcha00_elves_get_help"
#define WAIT_PROCESSES     "/xshcha00_wait_processes"
#define WAIT_MAIN          "/xshcha00_wait_main"

#define CLEAR_SEM(name, sem)   do { sem_close((sem)); sem_unlink((name)); } while(0)
#define CLEAR_SH_MEM(name, id) do { shmctl((id), IPC_RMID, NULL); shmdt((name)); } while(0)

typedef struct semaphores
{
	sem_t *help_from_santa;
	sem_t *wait_write_to_file;
	sem_t *santa_wake_up;
    sem_t *deer_wait;
	sem_t *wait_all_deers;
	sem_t *elves_get_help;
	sem_t *change_value;
	sem_t *wait_processes;
	sem_t *wait_main;
} semaphores_t;

typedef struct shared_vars
{
	FILE *fp;
	int  fp_id;

	int* elves_in_queue;
	int  elves_id;

	int* holiday;
	int  holiday_id;

	int* counter;
	int  counter_id;

    int* deers_left;
    int  deers_left_id;

	int* elf_get_help;
	int  elf_get_help_id;

	int* error_with_fork;
	int  error_id;

	int* elf;
	int  elf_id;

	int* deer;
	int  deer_id;

	int* proc;
	int  proc_id;
} sh_vars_t;

semaphores_t sems;
sh_vars_t    sh_vars;

typedef struct args
{
	int NE; // Number of elves 0 < NE < 1000
	int NR; // Number of reindeer 0 < NR < 20
	int TE; // Maximum time in milliseconds that the elve works independently 0 <= TE <= 1000
	int TR; // Maximum time in milliseconds that the reindeer returns home from vacation 0 <= TR <= 1000
} args_t;

/*
 * This function cheks enterred arguments.
 * In case of error function returns false, otherwise true.
 */
bool check_arguments(const char** argv, const int argc, args_t* args);

/*
 * This function allocates memory for shared variables.
 * In case of error function returns false, otherwise true.
 */
bool init_shared_memory(int** var, int* id);

/*
 * This function allocates semaphores.
 * In case of error function return false, otherwise true.
 */
bool init_semaphore(sem_t** sem, int init_value, string name);

/*
 * This function calls __init_semaphore__ and __init_shared_memory__.
 * Also it sets start values to shared variables.
 * In case of error function return false, otherwise true.
 */
bool init_vars();

/*
 * This function clears memory for shared variables and semaphores.
 */
void clear_res();

/*
 * This function creates file and allocates memory for it.
 * In case of error function return false, otherwise true.
 */
bool init_file();

#endif /* INIT_ARGS_H */

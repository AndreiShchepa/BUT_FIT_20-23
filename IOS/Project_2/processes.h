#ifndef PROCESSES_H
#define PROCESSES_H

#include "init_args.h"
#include <time.h>

#define PRINT_FORMAT_SANTA(sem, num, str)       do { sem_wait((sem)); fprintf(sh_vars.fp, "%d: Santa: %s\n",  (num)++ , (str)); fflush(sh_vars.fp); sem_post((sem)); } while(0)
#define PRINT_FORMAT_DEER(sem, num1, num2, str) do { sem_wait((sem)); fprintf(sh_vars.fp, "%d: RD %d: %s\n",  (num1)++, (num2), (str)); fflush(sh_vars.fp); sem_post((sem)); } while(0)
#define PRINT_FORMAT_ELF(sem, num1, num2, str)  do { sem_wait((sem)); fprintf(sh_vars.fp, "%d: Elf %d: %s\n", (num1)++, (num2), (str)); fflush(sh_vars.fp); sem_post((sem)); } while(0)

#define CHANGE_SH_VAR(sem, var, number)    do { sem_wait((sem)); (*(var)) += (number);  sem_post((sem)); } while(0)
#define INC_SH_VAR_VAR(sem, sh_var, var)   do { sem_wait((sem)); (var) = ++(*(sh_var)); sem_post((sem)); } while(0)
#define DEC_SH_VAR_VAR(sem, sh_var, var)   do { sem_wait((sem)); (var) = --(*(sh_var)); sem_post((sem)); } while(0)

enum name_of_proc
{
    SANTA,
    DEER,
    ELF,
    MAIN
};

/*
 * This function call function "sem_post()" for one semaphore n times (n > 0)
 */
void post_sem_n_times(sem_t** sem, int n);

/*
 * This function only for santa process.
 * Description in details you can find in this function in the file "processes.c".
 */
void santa_func(int num_of_deers);

/*
 * This function only for deer process.
 * At first deers have come to Santa and wait help from him.
 * Then Santa harnesses them and process of deer has ended.
 */
void deer_func(int num, int deer_time, int deers);

/*
 * This function only for elf process.
 * Description in details you can find in this function in the file "processes.c".
 */
void elf_func(int num, int elf_time);

#endif /* PROCESSES_H */

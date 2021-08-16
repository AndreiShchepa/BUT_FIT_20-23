
#include "init_args.h"

bool check_arguments(const char** argv, const int argc, args_t* args)
{
	char* trash = NULL;

    if (argc != 5)
	{
		fprintf(stderr, "%s", WRONG_NUMBER_ARGS);
        return false;
	}

    args->NE = strtol(argv[1], &trash, 10);
	if (*trash != '\0' || args->NE <= 0 || args->NE >= 1000)
	{
		fprintf(stderr, "%s", WRONG_ARG);
        return false;
	}

    args->NR = strtol(argv[2], &trash, 10);
	if (*trash != '\0' || args->NR <= 0 || args->NR >= 20)
	{
		fprintf(stderr, "%s", WRONG_ARG);
        return false;
	}

    args->TE = strtol(argv[3], &trash, 10);
	if (*trash != '\0' || args->TE < 0 || args->TE > 1000)
	{
		fprintf(stderr, "%s", WRONG_ARG);
        return false;
	}

    args->TR = strtol(argv[4], &trash, 10);
	if (*trash != '\0' || args->TR < 0 || args->TR > 1000)
	{
		fprintf(stderr, "%s", WRONG_ARG);
        return false;
	}

    return true;
}

bool init_shared_memory(int** var, int* id)
{
	if ((*id  = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | DEFFILEMODE)) == -1)
		return false;

	if ((*var = (int*) shmat(*id, NULL, 0)) == NULL)
		return false;

	return true;
}

bool init_semaphore(sem_t** sem, int init_value, string name)
{
	if ((*sem = sem_open(name, O_CREAT | O_EXCL, DEFFILEMODE, init_value)) == SEM_FAILED)
		return false;

	return true;
}

bool init_vars()
{

	if ((!init_shared_memory(&sh_vars.elves_in_queue,  &sh_vars.elves_id       ))
	 || (!init_shared_memory(&sh_vars.holiday,         &sh_vars.holiday_id     ))
	 || (!init_shared_memory(&sh_vars.counter,         &sh_vars.counter_id     ))
 	 || (!init_shared_memory(&sh_vars.deers_left,      &sh_vars.deers_left_id  ))
 	 || (!init_shared_memory(&sh_vars.elf_get_help,    &sh_vars.elf_get_help_id))
 	 || (!init_shared_memory(&sh_vars.error_with_fork, &sh_vars.error_id       ))
	 || (!init_shared_memory(&sh_vars.elf,             &sh_vars.elf_id         ))
	 || (!init_shared_memory(&sh_vars.deer,            &sh_vars.deer_id        ))
     || (!init_shared_memory(&sh_vars.proc,            &sh_vars.proc_id        )))
	{
		fprintf(stderr, "%s", ERR_SHARED_MEMORY);
		return false;
	}

	*sh_vars.counter = 1;
    *sh_vars.deers_left = 0;
    *sh_vars.elves_in_queue = 0;
    *sh_vars.holiday = 0;
	*sh_vars.elf_get_help = 0;
	*sh_vars.error_with_fork = 0;
	*sh_vars.elf = 0;
	*sh_vars.deer = 0;
	*sh_vars.proc = 0;

	if ((!init_semaphore(&sems.help_from_santa,    0, HELP_FROM_SANTA   ))
	 || (!init_semaphore(&sems.wait_write_to_file, 1, WAIT_WRITE_TO_FILE))
	 || (!init_semaphore(&sems.santa_wake_up,      0, SANTA_WAKE_UP     ))
	 || (!init_semaphore(&sems.deer_wait,          0, DEER_WAIT         ))
	 || (!init_semaphore(&sems.wait_all_deers,     0, WAIT_ALL_DEERS    ))
 	 || (!init_semaphore(&sems.elves_get_help,     0, ELVES_GET_HELP    ))
 	 || (!init_semaphore(&sems.change_value,       1, CHANGE_VALUE      ))
 	 || (!init_semaphore(&sems.wait_processes,     0, WAIT_PROCESSES    ))
  	 || (!init_semaphore(&sems.wait_main,          0, WAIT_MAIN         )))
	{
		fprintf(stderr, "%s", ERR_INIT_SEM);
		return false;
	}

	return true;
}

void clear_res()
{
	fclose(sh_vars.fp);

	CLEAR_SH_MEM(sh_vars.deers_left,      sh_vars.deers_left_id  );
	CLEAR_SH_MEM(sh_vars.fp,              sh_vars.fp_id          );
	CLEAR_SH_MEM(sh_vars.holiday,         sh_vars.holiday_id     );
	CLEAR_SH_MEM(sh_vars.counter,         sh_vars.counter_id     );
	CLEAR_SH_MEM(sh_vars.elves_in_queue,  sh_vars.elves_id       );
	CLEAR_SH_MEM(sh_vars.elf_get_help,    sh_vars.elf_get_help_id);
	CLEAR_SH_MEM(sh_vars.error_with_fork, sh_vars.error_id       );
	CLEAR_SH_MEM(sh_vars.elf,             sh_vars.elf_id         );
	CLEAR_SH_MEM(sh_vars.deer,            sh_vars.deer_id        );
	CLEAR_SH_MEM(sh_vars.proc,            sh_vars.proc_id        );

	CLEAR_SEM(SANTA_WAKE_UP,      sems.santa_wake_up     );
	CLEAR_SEM(HELP_FROM_SANTA,    sems.help_from_santa   );
	CLEAR_SEM(WAIT_WRITE_TO_FILE, sems.wait_write_to_file);
	CLEAR_SEM(DEER_WAIT,          sems.deer_wait         );
	CLEAR_SEM(WAIT_ALL_DEERS,     sems.wait_all_deers    );
	CLEAR_SEM(ELVES_GET_HELP,     sems.elves_get_help    );
	CLEAR_SEM(CHANGE_VALUE,       sems.change_value      );
	CLEAR_SEM(WAIT_PROCESSES,     sems.wait_processes    );
	CLEAR_SEM(WAIT_MAIN,          sems.wait_main         );
}

bool init_file()
{
	if ((sh_vars.fp_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | DEFFILEMODE)) == -1)
	{
		fprintf(stderr, "%s", ERR_SHARED_MEMORY);
		return false;
	}

	if ((sh_vars.fp = (FILE*) shmat(sh_vars.fp_id, NULL, 0)) == NULL)
	{
		shmctl(sh_vars.fp_id, IPC_RMID, NULL);
		fprintf(stderr, "%s", ERR_SHARED_MEMORY);
		return false;
	}

	if((sh_vars.fp = fopen("proj2.out", "w")) == NULL)
	{
		CLEAR_SH_MEM(sh_vars.fp, sh_vars.fp_id);
		fprintf(stderr, "%s\n", ERROR_WITH_FILE);
		return false;
	}

	return true;
}

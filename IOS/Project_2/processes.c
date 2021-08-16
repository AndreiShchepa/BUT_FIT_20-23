#include "processes.h"
#include "init_args.h"

void post_sem_n_times(sem_t** sem, int n)
{
	for (int i = 0; i < n; i++)
	{
		sem_post(*sem);
	}
}

void santa_func(int num_of_deers)
{
	PRINT_FORMAT_SANTA(sems.wait_write_to_file, *sh_vars.counter, "going to sleep");

start_santa:
	sem_wait(sems.santa_wake_up);

	// if all deers came to Santa, Santa must close workshop and go to harness reindeers
    if (*sh_vars.deers_left == num_of_deers) { goto end_santa; }

    PRINT_FORMAT_SANTA(sems.wait_write_to_file, *sh_vars.counter, "helping elves");

	// Santa helps to 3 elves
    post_sem_n_times(&sems.help_from_santa, 3);

	CHANGE_SH_VAR(sems.change_value, sh_vars.elves_in_queue, -3);

	// Santa cant go to slepp untill elves get help
    sem_wait(sems.elves_get_help);

    PRINT_FORMAT_SANTA(sems.wait_write_to_file, *sh_vars.counter, "going to sleep");
	goto start_santa;

end_santa:
    PRINT_FORMAT_SANTA(sems.wait_write_to_file, *sh_vars.counter, "closing workshop");

	// Santa close workshop
	CHANGE_SH_VAR(sems.change_value, sh_vars.holiday, 1);

	// ELves waiting a help go to the holidays
	post_sem_n_times(&sems.help_from_santa, 999);
	post_sem_n_times(&sems.deer_wait, num_of_deers);

	sem_wait(sems.wait_all_deers);

	// end of process santa
	PRINT_FORMAT_SANTA(sems.wait_write_to_file, *sh_vars.counter, "Christmas started");
}

void deer_func(int num, int deer_time, int deers)
{
	int count;

	PRINT_FORMAT_DEER(sems.wait_write_to_file, *sh_vars.counter, num, "rstarted");

    srand(time(NULL) * getpid());
    int time_for_sleep = deer_time/2 + (rand() % (deer_time/2 + 1));
    usleep(time_for_sleep*1000);

	PRINT_FORMAT_DEER(sems.wait_write_to_file, *sh_vars.counter, num, "return home");

	INC_SH_VAR_VAR(sems.change_value, sh_vars.deers_left, count);

	if (count == deers) { sem_post(sems.santa_wake_up); }

    sem_wait(sems.deer_wait);

	PRINT_FORMAT_DEER(sems.wait_write_to_file, *sh_vars.counter, num, "get hitched");

	DEC_SH_VAR_VAR(sems.change_value, sh_vars.deers_left, count);

	if (count == 0) { sem_post(sems.wait_all_deers); }
}

void elf_func(int num, int elf_time)
{
	int num_elves;
    int elves_in_q;

	PRINT_FORMAT_ELF(sems.wait_write_to_file, *sh_vars.counter, num, "started");

elf_start:
	srand(time(NULL) * getpid());
    int time_for_sleep = rand() % (elf_time + 1);
    usleep(time_for_sleep*1000);

	PRINT_FORMAT_ELF(sems.wait_write_to_file, *sh_vars.counter, num, "need help");

	INC_SH_VAR_VAR(sems.change_value, sh_vars.elves_in_queue, elves_in_q);

	// if number of elves 3*n, wake up Santa
	if (elves_in_q % 3 == 0 && elves_in_q != 0) { sem_post(sems.santa_wake_up); }

	sem_wait(sems.help_from_santa);

	if (*sh_vars.holiday == 0)
	{
		PRINT_FORMAT_ELF(sems.wait_write_to_file, *sh_vars.counter, num, "get help");

		INC_SH_VAR_VAR(sems.change_value, sh_vars.elf_get_help, num_elves);

		// if 3 elves have gotten help, Santa can go to sleep
		if (num_elves == 3)
        {
			CHANGE_SH_VAR(sems.change_value, sh_vars.elf_get_help, -3);
            sem_post(sems.elves_get_help);
        }

		goto elf_start;
	}

	PRINT_FORMAT_ELF(sems.wait_write_to_file, *sh_vars.counter, num, "taking holidays");
}

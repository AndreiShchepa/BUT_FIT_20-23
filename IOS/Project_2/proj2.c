#include "init_args.h"
#include "processes.h"
#include <errno.h>

int main(int argc, const char *argv[])
{
    int ret, count;
    args_t arguments;
	pid_t santa_proc, elf_proc, deer_proc;
    int name_proc = MAIN;

    if (!(ret = check_arguments(argv, argc, &arguments)))
		return 1;

    if (!(ret = init_file()))
		return 1;

	if (!(ret = init_vars()))
	{
		clear_res();
		return 1;
	}

    santa_proc = fork(); // generating of santa process
    if      (santa_proc == 0) { name_proc = SANTA;            }
    else if (santa_proc < 0)  { *sh_vars.error_with_fork = 1; }

    if (name_proc != MAIN) { goto queue_for_all_child_processes; }

    for (int i = 0; i < arguments.NE; i++)
    {
        elf_proc = fork(); // generating of elf processes
        if      (elf_proc == 0) { name_proc = ELF;               break; }
        else if (elf_proc < 0)  { *sh_vars.error_with_fork = 1;  break; }
    }

    if (name_proc != MAIN) { goto queue_for_all_child_processes; }

    for (int i = 0; i < arguments.NR; i++)
    {
        deer_proc = fork(); // generating of deer processes
        if      (deer_proc == 0) { name_proc = DEER;             break; }
        else if (deer_proc < 0)  { *sh_vars.error_with_fork = 1; break; }
    }

    if (name_proc == MAIN) { goto wait_untill_child_processes_work; }

queue_for_all_child_processes:
    CHANGE_SH_VAR(sems.change_value, sh_vars.proc, 1);

    if (*sh_vars.proc == arguments.NR + arguments.NE + 1)
        { post_sem_n_times(&sems.wait_processes, arguments.NR + arguments.NE + 1); }
    else if (*sh_vars.error_with_fork == 1)
        { post_sem_n_times(&sems.wait_processes, 1019); } // case error: 999 elves + 19 deers + 1 santa

    // wait all processes
    sem_wait(sems.wait_processes);

    if (*sh_vars.error_with_fork == 0)
    {
        if (name_proc == SANTA)
        {
            santa_func(arguments.NR);
        }
        else if (name_proc == ELF)
        {
            INC_SH_VAR_VAR(sems.change_value, sh_vars.elf, count);
            elf_func(count, arguments.TE);
        }
        else if (name_proc == DEER)
        {
            INC_SH_VAR_VAR(sems.change_value, sh_vars.deer, count);
            deer_func(count, arguments.TR, arguments.NR);
        }
    }

    DEC_SH_VAR_VAR(sems.change_value, sh_vars.proc, count);
    if (count == 0) { sem_post(sems.wait_main); }

    if (*sh_vars.error_with_fork == 0) { exit(0); }
    else                               { exit(1); }

wait_untill_child_processes_work:
    sem_wait(sems.wait_main);
    ret = *sh_vars.error_with_fork;
    clear_res();

    if (ret == 1)
    {
        fprintf(stderr, "%s", ERR_CREATE_CHILD);
        exit(1);
    }

    return 0;
}

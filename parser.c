#include "codexion.h"

int parse_args(int ac, char **av, t_sim *sim)
{

    int   i;
    int   y = 1;

	    if (ac != 9)
    {
        fprintf(stderr, "Error: wrong number of arguments\n");
        return 1;
    }

    while (y <= 7)
    {
        i = 0;
		if (!av[y] || av[y][0] == '\0')
        {
            fprintf(stderr, "Error: empty argument %d\n", y);
            return 1;
        }
        while (av[y][i])
        {
            if (av[y][i] < '0' || av[y][i] > '9')
            {
                fprintf(stderr, "Error: numeric arguments only (arg %d)\n", y);
                return 1;
            }
            i++;
        }
        y++;
    }

    sim->num_coders       = atoi(av[1]);
    sim->time_to_burnout   = atoi(av[2]);
    sim->time_to_compile   = atoi(av[3]);
    sim->time_to_debug     = atoi(av[4]);
    sim->time_to_refactor  = atoi(av[5]);
    sim->compiles_required = atoi(av[6]);
    sim->dongle_cooldown   = atoi(av[7]);
    sim->scheduler         = av[8];

    if (sim->num_coders <= 0 || sim->time_to_burnout <= 0 || sim->time_to_compile <= 0 ||
        sim->time_to_debug <= 0 || sim->time_to_refactor <= 0 || sim->compiles_required <= 0 ||
        sim->dongle_cooldown <= 0)
    {
        fprintf(stderr, "Error: wrong value\n");
        return 1;
    }
    if (!(strcmp(sim->scheduler, "fifo") == 0 ||
          strcmp(sim->scheduler, "edf") == 0))
    {
        fprintf(stderr, "Error: wrong word\n");
        return 1;
    }
    if (sim->num_coders < 2)
    {
        fprintf(stderr, "Error: should be at least 2\n");
        return 1;
    }

	return 0;
}

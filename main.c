#include "codexion.h"

void print_info(t_sim *sim)
{
    if (!sim)
        return;

    printf("coders: %d\n", sim->num_coders);
    printf("burnout: %ld\n", sim->time_to_burnout);
    printf("compile: %ld\n", sim->time_to_compile);
    printf("debug: %ld\n", sim->time_to_debug);
    printf("refactor: %ld\n", sim->time_to_refactor);
    printf("compiles required: %d\n", sim->compiles_required);
    printf("cooldown: %ld\n", sim->dongle_cooldown);
    printf("scheduler: %s\n", sim->scheduler);
    printf("current time: %ld ms\n", get_time_ms());
    usleep(500 * 1000);
    printf("current time: %ld ms\n", get_time_ms());
}

int main(int ac, char **av)
{
    t_sim sim;

    if (parse_args(ac, av, &sim) != 0)
        return 1;
    sim.dongles = malloc(sizeof(t_dongle) * sim.num_coders);
    if (!sim.dongles)
        return 1;
    if (init_sim(&sim) != 0)
    {
        free(sim.dongles);
        return 1;
    }
    if (init_dongles(&sim) != 0 || init_coders(&sim) != 0 ||
        run_simulation(&sim) != 0 || run_simulation(&sim) != 0)
    {
        clean_up(&sim);
        return 1;
    }
    print_info(&sim);
    clean_up(&sim);
    return 0;
}

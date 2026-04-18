#include "codexion.h"


int start_monitor(t_sim *sim, pthread_t *monitor)
{
    if (pthread_create(monitor, NULL, monitor_function, sim) != 0)
        return 1;
    return 0;
}
int start_coder_threads(t_sim *sim)
{
    int x = 0;
    while (x < sim->num_coders)
    {
        if (pthread_create(&sim->coders[x].thread, NULL, coder_function,
                           &sim->coders[x]) != 0)
        {
            int j = 0;
            while (j < x)
            {
                pthread_join(sim->coders[j].thread, NULL);
                j++;
            }
            return 1;
        }
        x++;
    }
    return 0;
}

void join_coder_threads(t_sim *sim)
{
    int x = 0;
    while (x < sim->num_coders)
    {
        pthread_join(sim->coders[x].thread, NULL);
        x++;
    }
}

int run_simulation(t_sim *sim)
{
    pthread_t monitor;
    if (start_monitor(sim, &monitor) != 0)
        return 1;

    if (start_coder_threads(sim) != 0)
    {
        pthread_mutex_lock(&sim->sim_mtx);
        sim->simulation_over = 1;
        pthread_mutex_unlock(&sim->sim_mtx);
        pthread_join(monitor, NULL);
        return 1;
    }

    join_coder_threads(sim);
    pthread_join(monitor, NULL);
    return 0;
}

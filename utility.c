#include "codexion.h"






long get_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

int coder_burned_out(t_sim *sim, int idx)
{
    int res = 0;
    pthread_mutex_lock(&sim->coder_mtx);
    if (get_time_ms() - sim->coders[idx].last_compile_start >=
        sim->time_to_burnout)
        res = 1;
    pthread_mutex_unlock(&sim->coder_mtx);
    return res;
}

int coder_finished(t_sim *sim, int idx)
{
    int res = 0;
    pthread_mutex_lock(&sim->coder_mtx);
    if (sim->coders[idx].compile_count >= sim->compiles_required)
        res = 1;
    pthread_mutex_unlock(&sim->coder_mtx);
    return res;
}

int monitor_iteration(t_sim *sim)
{
    int finished = 0;
    int y = 0;
    while (y < sim->num_coders)
    {
        if (coder_burned_out(sim, y))
        {
            log_action(sim, sim->coders[y].id, "burned out");
            pthread_mutex_lock(&sim->sim_mtx);
            sim->simulation_over = 1;
            pthread_mutex_unlock(&sim->sim_mtx);
            return 1;
        }
        if (coder_finished(sim, y))
            finished++;
        y++;
    }
    if (finished == sim->num_coders)
    {
        pthread_mutex_lock(&sim->sim_mtx);
        sim->simulation_over = 1;
        pthread_mutex_unlock(&sim->sim_mtx);
        return 1;
    }
    return 0;
}

void *monitor_function(void *arg)
{
    t_sim *sim = (t_sim *)arg;
    pthread_mutex_lock(&sim->sim_mtx);
    int over = sim->simulation_over;
    pthread_mutex_unlock(&sim->sim_mtx);
    while (over == 0)
    {
        if (monitor_iteration(sim))
            break;
        pthread_mutex_lock(&sim->sim_mtx);
        over = sim->simulation_over;
        pthread_mutex_unlock(&sim->sim_mtx);
        usleep(1000);
    }
    return NULL;
}



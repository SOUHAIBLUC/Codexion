#include "codexion.h"






long get_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

void *monitor_function(void *arg)
{
    t_sim *sim = (t_sim *)arg;
    pthread_mutex_lock(&sim->sim_mtx);
    int over = sim->simulation_over;
    pthread_mutex_unlock(&sim->sim_mtx);
    while (over == 0)
    {
        int    i = 0;
        int y = 0;
        while (y < sim->num_coders)
        {
            pthread_mutex_lock(&sim->coder_mtx);
            if (get_time_ms() - sim->coders[y].last_compile_start >=
                sim->time_to_burnout)
            {
                pthread_mutex_unlock(&sim->coder_mtx);
                log_action(sim, sim->coders[y].id, "burned out");
                pthread_mutex_lock(&sim->sim_mtx);
                sim->simulation_over = 1;
                over                 = 1;
                pthread_mutex_unlock(&sim->sim_mtx);
                break;
            }
            if (sim->coders[y].compile_count >= sim->compiles_required)
                i++;
            pthread_mutex_unlock(&sim->coder_mtx);
            y++;
        }
        if (i == sim->num_coders)
        {
            pthread_mutex_lock(&sim->sim_mtx);
            sim->simulation_over = 1;
            pthread_mutex_unlock(&sim->sim_mtx);
        }
        pthread_mutex_lock(&sim->sim_mtx);
        over = sim->simulation_over;
        pthread_mutex_unlock(&sim->sim_mtx);
        usleep(1000);
    }
    return NULL;
}

void clean_up(t_sim *sim)
{

    if (!sim)
        return;

    int i = 0;
    if (sim->dongles)
    {
        while (i < sim->num_coders)
        {
            if (sim->dongles[i].queue)
            {
                pthread_cond_destroy(&sim->dongles[i].cond);
                pthread_mutex_destroy(&sim->dongles[i].mtx);
                free(sim->dongles[i].queue);
                sim->dongles[i].queue = NULL;
            }
            i++;
        }
        free(sim->dongles);
        sim->dongles = NULL;
    }

    /* destroy mutexes if they were initialized */
    pthread_mutex_destroy(&sim->sim_mtx);
    pthread_mutex_destroy(&sim->coder_mtx);
    pthread_mutex_destroy(&sim->log_mtx);

    if (sim->coders)
    {
        free(sim->coders);
        sim->coders = NULL;
    }
}
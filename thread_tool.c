#include "codexion.h"

void log_action(t_sim *sim, int id, char *action)
{
    pthread_mutex_lock(&sim->log_mtx);
    pthread_mutex_lock(&sim->sim_mtx);
    int over = sim->simulation_over;
    pthread_mutex_unlock(&sim->sim_mtx);
    if (over == 1)
    {
        pthread_mutex_unlock(&sim->log_mtx);
        return;
    }
    printf("%ld %d %s\n", get_time_ms() - sim->start_time, id, action);
    pthread_mutex_unlock(&sim->log_mtx);
}

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
        size_t y = 0;
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

    int i = 0;
    while (i < sim->num_coders)
    {

        pthread_mutex_destroy(&sim->dongles[i].mtx);
        i++;
    }
    // destroy in cleanup
    pthread_mutex_destroy(&sim->sim_mtx);
    pthread_mutex_destroy(&sim->coder_mtx);
    pthread_mutex_destroy(&sim->log_mtx);
    free(sim->dongles);
    free(sim->coders);
}
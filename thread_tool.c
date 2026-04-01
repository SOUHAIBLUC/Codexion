#include "codexion.h"

void log_action(t_sim *sim, int id, char *action)
{
    pthread_mutex_lock(&sim->log_mtx);
    if (sim->simulation_over == 1)
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
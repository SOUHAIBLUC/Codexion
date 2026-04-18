#include "codexion.h"

void *coder_function(void *arg)
{
    t_dongle *first;
    t_dongle *secnd;
    t_coder  *coder    = (t_coder *)arg;
    long      cooldown = coder->sim->dongle_cooldown;

    if (coder->right->id > coder->left->id)
    {
        first = coder->left;
        secnd = coder->right;
    }
    else
    {
        first = coder->right;
        secnd = coder->left;
    }

    if (coder->id % 2 == 0)
        usleep(10);
    pthread_mutex_lock(&coder->sim->sim_mtx);
    int over = coder->sim->simulation_over;
    pthread_mutex_unlock(&coder->sim->sim_mtx);
    while (over == 0)
    {
        if (strcmp(coder->sim->scheduler, "fifo") == 0)
            acquire_dongles_fifo(coder, first, secnd, cooldown);
        else
            acquire_dongles_edf(coder, first, secnd, cooldown);

        log_action(coder->sim, coder->id, "has taken a dongle");
        log_action(coder->sim, coder->id, "has taken a dongle");
        // compile
        log_action(coder->sim, coder->id, "is compiling");
        pthread_mutex_lock(&coder->sim->coder_mtx);
        coder->last_compile_start = get_time_ms();

        pthread_mutex_unlock(&coder->sim->coder_mtx);
        usleep(coder->sim->time_to_compile * 1000);
        pthread_mutex_lock(&coder->sim->coder_mtx);
        coder->compile_count++;
        pthread_mutex_unlock(&coder->sim->coder_mtx);
        // debuging
        log_action(coder->sim, coder->id, "is debugging");
        usleep(coder->sim->time_to_debug * 1000);
        // refactoring
        log_action(coder->sim, coder->id, "is refactoring");
        usleep(coder->sim->time_to_refactor * 1000);

        if (strcmp(coder->sim->scheduler, "fifo") == 0)
            release_dongles_fifo(coder, first, secnd);
        else
            release_dongles_edf(coder, first, secnd);

        pthread_mutex_lock(&coder->sim->sim_mtx);
        over = coder->sim->simulation_over;
        pthread_mutex_unlock(&coder->sim->sim_mtx);
    }
    pthread_mutex_lock(&coder->sim->sim_mtx);
    coder->sim->simulation_over = 1;
    pthread_mutex_unlock(&coder->sim->sim_mtx);
    return NULL;
}

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

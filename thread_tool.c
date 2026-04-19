#include "codexion.h"

void *coder_function(void *arg)
{
    t_dongle *first;
    t_dongle *secnd;
    t_coder  *coder    = (t_coder *)arg;
    long      cooldown = coder->sim->dongle_cooldown;

    init_coder_order(coder, &first, &secnd);
    if (coder->id % 2 == 0)
        usleep(10);
    pthread_mutex_lock(&coder->sim->sim_mtx);
    int over = coder->sim->simulation_over;
    pthread_mutex_unlock(&coder->sim->sim_mtx);
    while (over == 0)
    {
        acquire_dongles(coder, first, secnd, cooldown);
        pthread_mutex_lock(&coder->sim->sim_mtx);
        over = coder->sim->simulation_over;
        pthread_mutex_unlock(&coder->sim->sim_mtx);
        if (over)
        {
            release_dongles(coder, first, secnd);
            break;
        }
        if (coder_work(coder))
        {
            release_dongles(coder, first, secnd);
            break;
        }
        release_dongles(coder, first, secnd);
        pthread_mutex_lock(&coder->sim->sim_mtx);
        over = coder->sim->simulation_over;
        pthread_mutex_unlock(&coder->sim->sim_mtx);
    }
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

void init_coder_order(t_coder *coder, t_dongle **first, t_dongle **secnd)
{
    if (coder->right->id > coder->left->id)
    {
        *first = coder->left;
        *secnd = coder->right;
    }
    else
    {
        *first = coder->right;
        *secnd = coder->left;
    }
}

int coder_work(t_coder *coder)
{
    log_action(coder->sim, coder->id, "has taken a dongle");
    log_action(coder->sim, coder->id, "has taken a dongle");
    pthread_mutex_lock(&coder->sim->sim_mtx);
    int over = coder->sim->simulation_over;
    pthread_mutex_unlock(&coder->sim->sim_mtx);
    if (over)
        return 1;
    pthread_mutex_lock(&coder->sim->coder_mtx);
    int already_done = (coder->compile_count >= coder->sim->compiles_required);
    pthread_mutex_unlock(&coder->sim->coder_mtx);
    if (already_done)
        return 1;
    log_action(coder->sim, coder->id, "is compiling");
    pthread_mutex_lock(&coder->sim->coder_mtx);
    coder->last_compile_start = get_time_ms();
    pthread_mutex_unlock(&coder->sim->coder_mtx);
    usleep(coder->sim->time_to_compile * 1000);
    pthread_mutex_lock(&coder->sim->coder_mtx);
    coder->compile_count++;
    pthread_mutex_unlock(&coder->sim->coder_mtx);
    log_action(coder->sim, coder->id, "is debugging");
    usleep(coder->sim->time_to_debug * 1000);
    log_action(coder->sim, coder->id, "is refactoring");
    usleep(coder->sim->time_to_refactor * 1000);
    return 0;
}

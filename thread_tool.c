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
        pthread_mutex_lock(&first->mtx);
        first->queue[first->tail] = coder->id;
        first->tail               = (first->tail + 1) % coder->sim->num_coders;
        first->queue_siz++;
        pthread_mutex_unlock(&first->mtx);
        pthread_mutex_lock(&secnd->mtx);
        secnd->queue[secnd->tail] = coder->id;
        secnd->tail               = (secnd->tail + 1) % coder->sim->num_coders;
        secnd->queue_siz++;
        pthread_mutex_unlock(&secnd->mtx);
        pthread_mutex_lock(&first->mtx);
        while (first->queue[first->head] != coder->id ||
               get_time_ms() - first->released_at < cooldown)
        {
            pthread_mutex_unlock(&first->mtx);
            usleep(1000);
            pthread_mutex_lock(&first->mtx);
        }
        pthread_mutex_lock(&secnd->mtx);
        while (secnd->queue[secnd->head] != coder->id ||
               get_time_ms() - secnd->released_at < cooldown)
        {
            pthread_mutex_unlock(&secnd->mtx);
            usleep(1000);
            pthread_mutex_lock(&secnd->mtx);
        }

        //pthread_mutex_unlock(&first->mtx);
        //pthread_mutex_unlock(&secnd->mtx);

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
        // first dangle

        first->head = (first->head + 1) % coder->sim->num_coders;
        first->queue_siz--;
        first->released_at = get_time_ms();
        pthread_cond_broadcast(&first->cond);
        pthread_mutex_unlock(&first->mtx);
        // secnd dongle

        secnd->head = (secnd->head + 1) % coder->sim->num_coders;
        secnd->queue_siz--;
        secnd->released_at = get_time_ms();
        pthread_cond_broadcast(&secnd->cond);
        pthread_mutex_unlock(&secnd->mtx);
        // debuging
        log_action(coder->sim, coder->id, "is debugging");
        usleep(coder->sim->time_to_debug * 1000);
        // refactoring
        log_action(coder->sim, coder->id, "is refactoring");
        usleep(coder->sim->time_to_refactor * 1000);

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


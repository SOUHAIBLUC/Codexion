#include "codexion.h"

void acquire_dongles_fifo(
    t_coder *coder, t_dongle *first, t_dongle *secnd, long cooldown
)
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
}

void acquire_dongles_edf(
    t_coder *coder, t_dongle *first, t_dongle *secnd, long cooldown
)
{
    pthread_mutex_lock(&first->mtx);
    pthread_mutex_lock(&coder->sim->coder_mtx);
    long deadline = coder->last_compile_start + coder->sim->time_to_burnout;
    pthread_mutex_unlock(&coder->sim->coder_mtx);
    heap_insert(first, coder->id, deadline);
    pthread_mutex_unlock(&first->mtx);
    pthread_mutex_lock(&secnd->mtx);
    heap_insert(secnd, coder->id, deadline);
    pthread_mutex_unlock(&secnd->mtx);
    // waiting
    pthread_mutex_lock(&first->mtx);
    while (heap_peek(first) != coder->id ||
           get_time_ms() - first->released_at < cooldown)
    {
        pthread_mutex_unlock(&first->mtx);
        usleep(1000);
        pthread_mutex_lock(&first->mtx);
    }
    pthread_mutex_lock(&secnd->mtx);
    while (heap_peek(secnd) != coder->id ||
           get_time_ms() - secnd->released_at < cooldown)
    {
        pthread_mutex_unlock(&secnd->mtx);
        usleep(1000);
        pthread_mutex_lock(&secnd->mtx);
    }
}

void release_dongles_fifo(t_coder *coder, t_dongle *first, t_dongle *secnd)
{
    first->head = (first->head + 1) % coder->sim->num_coders;
    first->queue_siz--;
    first->released_at = get_time_ms();
    pthread_cond_broadcast(&first->cond);
    pthread_mutex_unlock(&first->mtx);

    secnd->head = (secnd->head + 1) % coder->sim->num_coders;
    secnd->queue_siz--;
    secnd->released_at = get_time_ms();
    pthread_cond_broadcast(&secnd->cond);
    pthread_mutex_unlock(&secnd->mtx);
}

void release_dongles_edf(t_coder *coder, t_dongle *first, t_dongle *secnd)
{
    heap_remove(first);
    first->released_at = get_time_ms();
    pthread_cond_broadcast(&first->cond);
    pthread_mutex_unlock(&first->mtx);
    // secnd
    heap_remove(secnd);
    secnd->released_at = get_time_ms();
    pthread_cond_broadcast(&secnd->cond);
    pthread_mutex_unlock(&secnd->mtx);
}
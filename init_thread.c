#include "codexion.h"

int init_sim(t_sim *sim)
{
    pthread_mutex_init(&sim->log_mtx, NULL);
    pthread_mutex_init(&sim->sim_mtx, NULL);
    sim->start_time = get_time_ms();

    return 0;
}

int init_dongles(t_sim *sim)
{
    int i;

    i = 0;
    while (i < sim->num_coders)
    {
        sim->dongles[i].queue = NULL;
        i++;
    }

    i = 0;
    while (i < sim->num_coders)
    {
        sim->dongles[i].heap = calloc(sim->num_coders, sizeof(t_heap_entry));
        if (!sim->dongles[i].heap)
        {
            cleanup_dongles_partial(sim, i);
            return 1;
        }
        sim->dongles[i].queue = calloc(sim->num_coders, sizeof(int));
        if (!sim->dongles[i].queue)
        {
            cleanup_dongles_partial(sim, i);
            return 1;
        }
        pthread_mutex_init(&sim->dongles[i].mtx, NULL);
        pthread_cond_init(&sim->dongles[i].cond, NULL);

        sim->dongles[i].heap_size   = 0;
        sim->dongles[i].head        = 0;
        sim->dongles[i].tail        = 0;
        sim->dongles[i].queue_siz   = 0;
        sim->dongles[i].released_at = 0;
        sim->dongles[i].available   = 1;
        sim->dongles[i].id          = i;

        i++;
    }
    return 0;
}

void destroy_dongle(t_dongle *d)
{
    if (!d || !d->queue)
        return;
    pthread_cond_destroy(&d->cond);
    pthread_mutex_destroy(&d->mtx);
    free(d->queue);
    if (d->heap)
        free(d->heap);
    d->queue = NULL;
    d->heap  = NULL;
}

void cleanup_dongles_partial(t_sim *sim, int count)
{
    int k = 0;
    while (k < count)
    {
        destroy_dongle(&sim->dongles[k]);
        k++;
    }
    if (sim->dongles)
    {
        free(sim->dongles);
        sim->dongles = NULL;
    }
}

int init_coders(t_sim *sim)
{
    pthread_mutex_init(&sim->coder_mtx, NULL);
    sim->start_time = get_time_ms();
    sim->coders     = (t_coder *)calloc(sizeof(t_coder) * sim->num_coders, 1);
    if (!sim->coders)
        return 1;

    int j = 0;
    while (j < sim->num_coders)
    {

        sim->coders[j].id                 = j;
        sim->coders[j].last_compile_start = sim->start_time;
        sim->coders[j].left               = &sim->dongles[j];
        sim->coders[j].right = &sim->dongles[(j + 1) % sim->num_coders];
        sim->coders[j].sim   = sim;
        j++;
    }
    sim->simulation_over = 0;
    return 0;
}
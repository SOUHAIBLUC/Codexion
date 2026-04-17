#include "codexion.h"

int init_sim(t_sim *sim)
{
    pthread_mutex_init(&sim->log_mtx, NULL);
    pthread_mutex_init(&sim->sim_mtx, NULL);
    sim->start_time = get_time_ms();
    sim->coders     = calloc(sizeof(t_coder) * sim->num_coders, 1);
    if (!sim->coders)
        return 1;
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
        sim->dongles[i].queue = calloc(sim->num_coders, sizeof(int));
        if (!sim->dongles[i].queue)
        {
            int k = 0;
            while (k < i)
            {
                if (sim->dongles[k].queue)
                {
                    pthread_cond_destroy(&sim->dongles[k].cond);
                    pthread_mutex_destroy(&sim->dongles[k].mtx);
                    free(sim->dongles[k].queue);
                    sim->dongles[k].queue = NULL;
                }
                k++;
            }
            free(sim->dongles);
            sim->dongles = NULL;
            return 1;
        }
        pthread_mutex_init(&sim->dongles[i].mtx, NULL);
        pthread_cond_init(&sim->dongles[i].cond, NULL);
        sim->dongles[i].head        = 0;
        sim->dongles[i].tail        = 0;
        sim->dongles[i].queue_siz   = 0;
        sim->dongles[i].released_at = 0;
        sim->dongles[i].available   = 1;
        sim->dongles[i].id = i;

        i++;
    }
    return 0;
}
void destroy_dongle(t_dongle *d)
{
    if (!d || !d->queue) return;
    pthread_cond_destroy(&d->cond);
    pthread_mutex_destroy(&d->mtx);
    free(d->queue);
    d->queue = NULL;
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
        sim->coders[j].last_compile_start = get_time_ms();
        sim->coders[j].left               = &sim->dongles[j];
        sim->coders[j].right = &sim->dongles[(j + 1) % sim->num_coders];
        sim->coders[j].sim   = sim;
        j++;
    }
    sim->simulation_over = 0;
	return 0;
}
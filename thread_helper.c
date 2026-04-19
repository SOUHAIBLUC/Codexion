#include "codexion.h"


void clean_up(t_sim *sim)
{

    if (!sim)
        return;

    int i = 0;
    if (sim->dongles)
    {
        while (i < sim->num_coders)
        {
            destroy_dongle(&sim->dongles[i]);
            free(sim->dongles[i].heap);
            i++;
        }
        free(sim->dongles);
        sim->dongles = NULL;
    }
    pthread_mutex_destroy(&sim->sim_mtx);
    pthread_mutex_destroy(&sim->coder_mtx);
    pthread_mutex_destroy(&sim->log_mtx);

    if (sim->coders)
    {
        free(sim->coders);
        sim->coders = NULL;
    }
}
void acquire_dongles(t_coder *coder, t_dongle *first, t_dongle *secnd, long cooldown)
{
    if (strcmp(coder->sim->scheduler, "fifo") == 0)
        acquire_dongles_fifo(coder, first, secnd, cooldown);
    else
        acquire_dongles_edf(coder, first, secnd, cooldown);
}

void release_dongles(t_coder *coder, t_dongle *first, t_dongle *secnd)
{
    if (strcmp(coder->sim->scheduler, "fifo") == 0)
        release_dongles_fifo(coder, first, secnd);
    else
        release_dongles_edf(first, secnd);
}
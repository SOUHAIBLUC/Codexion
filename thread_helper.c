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
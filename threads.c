#include "codexion.h"


int start_monitor(t_sim *sim, pthread_t *monitor)
{
    int       x = 0;
	pthread_create(&monitor, NULL, monitor_function, &sim);
        while (x < sim->num_coders)
        {
            pthread_create(
                &sim->coders[x].thread, NULL, coder_function, &sim->coders[x]
            );
            x++;
        }
        x = 0;

        while (x < sim->num_coders)
        {
            pthread_join(sim->coders[x].thread, NULL);
            x++;
        }
        pthread_join(monitor, NULL);
	return 0;
}
int start_coder_threads(t_sim *sim)
{

}
void join_coder_threads(t_sim *sim)
{

}
int run_simulation(t_sim *sim)
{

}
#include "codexion.h"

void *coder_function(void *arg)
{
    t_dongle *first;
    t_dongle *secnd;
    t_coder  *coder = (t_coder *)arg;

    if (coder->right > coder->left)
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
        long cooldown = coder->sim->dongle_cooldown;
        pthread_mutex_lock(&first->mtx);
        long released = first->released_at;
        pthread_mutex_unlock(&first->mtx);
        while (get_time_ms() - released < cooldown)
        {
            usleep(100);
            pthread_mutex_lock(&first->mtx);
            released = first->released_at;
            pthread_mutex_unlock(&first->mtx);
        }
        pthread_mutex_lock(&secnd->mtx);
        released = secnd->released_at;
        pthread_mutex_unlock(&secnd->mtx);
        while (get_time_ms() - secnd->released_at < cooldown)
        {
            usleep(100);
            pthread_mutex_lock(&secnd->mtx);
            released = secnd->released_at;
            pthread_mutex_unlock(&secnd->mtx);
        }

        pthread_mutex_lock(&first->mtx);
        pthread_mutex_lock(&secnd->mtx);
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

        pthread_mutex_unlock(&first->mtx);
        pthread_mutex_unlock(&secnd->mtx);
        pthread_mutex_lock(&first->mtx);
        first->released_at = get_time_ms();
        pthread_mutex_unlock(&first->mtx);
        pthread_mutex_lock(&secnd->mtx);
        secnd->released_at = get_time_ms();
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

int main(int ac, char **av)
{

    pthread_t monitor;
    int       x = 0;
    t_sim     sim;
    int       i;
    int       y = 1;

    if (ac == 9)
    {

        while (y < 7)
        {
            i = 0;
            while (av[y][i])
            {
                if (av[y][i] < '0' || av[y][i] > '9')
                {
                    printf("Error: no alpha in the arguments\n");
                    return 0;
                }
                i++;
            }
            y++;
        }

        sim.num_coders        = atoi(av[1]);
        sim.time_to_burnout   = atoi(av[2]);
        sim.time_to_compile   = atoi(av[3]);
        sim.time_to_debug     = atoi(av[4]);
        sim.time_to_refactor  = atoi(av[5]);
        sim.compiles_required = atoi(av[6]);
        sim.dongle_cooldown   = atoi(av[7]);
        sim.scheduler         = av[8];

        if (atoi(av[1]) <= 0 || atoi(av[2]) <= 0 || atoi(av[3]) <= 0 ||
            atoi(av[4]) <= 0 || atoi(av[5]) <= 0 || atoi(av[6]) <= 0 ||
            atoi(av[7]) <= 0)
        {
            printf("Error: wrong value\n");
            return 0;
        }
        if (!(strcmp(sim.scheduler, "fifo") == 0 ||
              strcmp(sim.scheduler, "edf") == 0))
        {
            printf("Error: wrong word\n");
            return 0;
        }
        if (sim.num_coders < 2)
        {
            printf("Error: should be at least 2\n");
            return 0;
        }

        sim.dongles = malloc(sizeof(t_dongle) * sim.num_coders);
        if (!sim.dongles)
            return 1;
        i = 0;
        while (i < sim.num_coders)
        {
            pthread_mutex_init(&sim.dongles[i].mtx, NULL);
            sim.dongles[i].available   = 1;
            sim.dongles[i].released_at = 0;
            i++;
        }

        pthread_mutex_init(&sim.sim_mtx, NULL);
        pthread_mutex_init(&sim.coder_mtx, NULL);
        pthread_mutex_init(&sim.log_mtx, NULL);

        sim.start_time = get_time_ms();
        sim.coders     = (t_coder *)calloc(sizeof(t_coder) * sim.num_coders, 1);
        if (!sim.coders)
            return 1;

        int j = 0;
        while (j < sim.num_coders)
        {

            sim.coders[j].id                 = j;
            sim.coders[j].last_compile_start = get_time_ms();
            sim.coders[j].left               = &sim.dongles[j];
            sim.coders[j].right = &sim.dongles[(j + 1) % sim.num_coders];
            sim.coders[j].sim   = &sim;
            j++;
        }
        sim.simulation_over = 0;

        pthread_create(&monitor, NULL, monitor_function, &sim);
        while (x < sim.num_coders)
        {
            pthread_create(
                &sim.coders[x].thread, NULL, coder_function, &sim.coders[x]
            );
            x++;
        }
        x = 0;

        while (x < sim.num_coders)
        {
            pthread_join(sim.coders[x].thread, NULL);
            x++;
        }
        pthread_join(monitor, NULL);

        printf("coders: %d\n", sim.num_coders);
        printf("burnout: %ld\n", sim.time_to_burnout);
        printf("compile: %ld\n", sim.time_to_compile);
        printf("debug: %ld\n", sim.time_to_debug);
        printf("refactor: %ld\n", sim.time_to_refactor);
        printf("compiles required: %d\n", sim.compiles_required);
        printf("cooldown: %ld\n", sim.dongle_cooldown);
        printf("scheduler: %s\n", sim.scheduler);
        printf("current time: %ld ms\n", get_time_ms());
        usleep(500 * 1000); // sleep 500ms
        printf("current time: %ld ms\n", get_time_ms());

        clean_up(&sim);
    }
    else
    {
        printf("Error\n");
        return 0;
    }
}
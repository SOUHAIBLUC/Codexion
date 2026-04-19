#ifndef CODEXION_H
#define CODEXION_H

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

// forward declarations
typedef struct s_dongle     t_dongle;
typedef struct s_sim        t_sim;
typedef struct s_coder      t_coder;
typedef struct s_heap_entry t_heap_entry;

struct s_heap_entry
{
    int  coder_id;
    long deadline;
};

struct s_dongle
{
    pthread_cond_t  cond;
    pthread_mutex_t mtx;
    t_heap_entry   *heap;
    int             heap_size;
    int            *queue;
    int             head;
    int             tail;
    int             queue_siz;
    int             available;
    long            released_at;
    int             id;
};

struct s_sim
{
    int             num_coders;
    long            time_to_burnout;
    long            time_to_compile;
    long            time_to_debug;
    long            time_to_refactor;
    int             compiles_required;
    long            dongle_cooldown;
    char           *scheduler;
    int             simulation_over;
    pthread_mutex_t log_mtx;
    pthread_mutex_t sim_mtx;
    pthread_mutex_t coder_mtx;
    long            start_time;
    t_dongle       *dongles;
    t_coder        *coders;
};

struct s_coder
{
    int       id;
    pthread_t thread;
    long      last_compile_start;
    int       compile_count;
    t_dongle *left;
    t_dongle *right;
    t_sim    *sim;
    bool      start;
};

long  get_time_ms(void);
void  log_action(t_sim *sim, int id, char *action);
void *monitor_function(void *arg);
void  clean_up(t_sim *sim);
void  destroy_dongle(t_dongle *d);
void  cleanup_dongles_partial(t_sim *sim, int count);
int   coder_burned_out(t_sim *sim, int idx);
int   coder_finished(t_sim *sim, int idx);
int   monitor_iteration(t_sim *sim);
void *coder_function(void *arg);
int   parse_args(int ac, char **av, t_sim *sim);
int   init_sim(t_sim *sim);
int   init_dongles(t_sim *sim);
int   init_coders(t_sim *sim);
int   run_simulation(t_sim *sim);
void  heap_insert(t_dongle *d, int coder_id, long deadline);
void  heap_remove(t_dongle *d);
int   heap_peek(t_dongle *d);
void  acquire_dongles_fifo(
    t_coder *coder, t_dongle *first, t_dongle *secnd, long cooldown
);
void acquire_dongles_edf(
    t_coder *coder, t_dongle *first, t_dongle *secnd, long cooldown
);
void release_dongles_fifo(t_coder *coder, t_dongle *first, t_dongle *secnd);
void release_dongles_edf(t_dongle *first, t_dongle *secnd);
int coder_work(t_coder *coder);
void init_coder_order(t_coder *coder, t_dongle **first, t_dongle **secnd);
void acquire_dongles(
    t_coder *coder, t_dongle *first, t_dongle *secnd, long cooldown
);
void release_dongles(t_coder *coder, t_dongle *first, t_dongle *secnd);

#endif
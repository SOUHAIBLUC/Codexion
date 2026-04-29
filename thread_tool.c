/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread_tool.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: so-ait-l <so-ait-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 09:38:22 by so-ait-l          #+#    #+#             */
/*   Updated: 2026/04/26 10:45:44 by so-ait-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int coder_cycle(t_coder *coder, t_dongle *first,
        t_dongle *secnd, long cooldown)
{
    acquire_dongles(coder, first, secnd, cooldown);
    if (get_over(coder->sim))
    {
        release_dongles(coder, first, secnd);
        return (1);
    }
    if (coder_work(coder))
    {
        release_dongles(coder, first, secnd);
        return (1);
    }
    release_dongles(coder, first, secnd); 
    log_action(coder->sim, coder->id, "is debugging");
    usleep(coder->sim->time_to_debug * 1000);
    log_action(coder->sim, coder->id, "is refactoring");
    usleep(coder->sim->time_to_refactor * 1000);
    return (0);
}

void	*coder_function(void *arg)
{
	t_dongle	*first;
	t_dongle	*secnd;
	t_coder		*coder;
	long		cooldown;

	coder = (t_coder *)arg;
	cooldown = coder->sim->dongle_cooldown;
	init_coder_order(coder, &first, &secnd);
	if (coder->id % 2 == 0)
		usleep(10);
	while (!get_over(coder->sim))
		if (coder_cycle(coder, first, secnd, cooldown))
			break ;
	return (NULL);
}

void	init_coder_order(t_coder *coder, t_dongle **first, t_dongle **secnd)
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

static void	do_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->coder_mtx);
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&coder->sim->coder_mtx);
	usleep(coder->sim->time_to_compile * 1000);
	pthread_mutex_lock(&coder->sim->coder_mtx);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->sim->coder_mtx);
}

int coder_work(t_coder *coder)
{
    int already_done;

    log_action(coder->sim, coder->id, "has taken a dongle");
    log_action(coder->sim, coder->id, "has taken a dongle");
    if (get_over(coder->sim))
        return (1);
    pthread_mutex_lock(&coder->sim->coder_mtx);
    already_done = (coder->compile_count >= coder->sim->compiles_required);
    pthread_mutex_unlock(&coder->sim->coder_mtx);
    if (already_done)
        return (1);
    log_action(coder->sim, coder->id, "is compiling");
    do_compile(coder);
    return (0);   // ← return here, before debug/refactor!
}

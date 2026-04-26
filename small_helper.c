/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   small_helper.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: so-ait-l <so-ait-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 10:13:20 by so-ait-l          #+#    #+#             */
/*   Updated: 2026/04/26 10:48:30 by so-ait-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cleanup_dongles_partial(t_sim *sim, int count)
{
	int	k;

	k = 0;
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

void	edf_enqueue(t_coder *coder, t_dongle *first, t_dongle *secnd)
{
	long	deadline;

	pthread_mutex_lock(&first->mtx);
	pthread_mutex_lock(&coder->sim->coder_mtx);
	deadline = coder->last_compile_start + coder->sim->time_to_burnout;
	pthread_mutex_unlock(&coder->sim->coder_mtx);
	heap_insert(first, coder->id, deadline);
	pthread_mutex_unlock(&first->mtx);
	pthread_mutex_lock(&secnd->mtx);
	heap_insert(secnd, coder->id, deadline);
	pthread_mutex_unlock(&secnd->mtx);
	(void)deadline;
}

void	fifo_enqueue(t_coder *coder, t_dongle *first, t_dongle *secnd)
{
	pthread_mutex_lock(&first->mtx);
	first->queue[first->tail] = coder->id;
	first->tail = (first->tail + 1) % coder->sim->num_coders;
	first->queue_siz++;
	pthread_mutex_unlock(&first->mtx);
	pthread_mutex_lock(&secnd->mtx);
	secnd->queue[secnd->tail] = coder->id;
	secnd->tail = (secnd->tail + 1) % coder->sim->num_coders;
	secnd->queue_siz++;
	pthread_mutex_unlock(&secnd->mtx);
}

void	release_dongles_edf(t_dongle *first, t_dongle *secnd)
{
	heap_remove(first);
	first->released_at = get_time_ms();
	pthread_cond_broadcast(&first->cond);
	pthread_mutex_unlock(&first->mtx);
	heap_remove(secnd);
	secnd->released_at = get_time_ms();
	pthread_cond_broadcast(&secnd->cond);
	pthread_mutex_unlock(&secnd->mtx);
}

void	log_action(t_sim *sim, int id, char *action)
{
	int	over;

	pthread_mutex_lock(&sim->log_mtx);
	pthread_mutex_lock(&sim->sim_mtx);
	over = sim->simulation_over;
	pthread_mutex_unlock(&sim->sim_mtx);
	if (over == 1)
	{
		pthread_mutex_unlock(&sim->log_mtx);
		return ;
	}
	printf("%ld %d %s\n", get_time_ms() - sim->start_time, id, action);
	pthread_mutex_unlock(&sim->log_mtx);
}

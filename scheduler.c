/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: so-ait-l <so-ait-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 09:38:10 by so-ait-l          #+#    #+#             */
/*   Updated: 2026/04/26 10:52:52 by so-ait-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	edf_enqueue(t_coder *coder, t_dongle *first, t_dongle *secnd);

void	acquire_dongles_fifo(t_coder *coder, t_dongle *first, t_dongle *secnd,
		long cooldown)
{
	fifo_enqueue(coder, first, secnd);
	fifo_wait_for_turn(coder, first, secnd, cooldown);
}

static void	edf_wait_for_turn(t_coder *coder, t_dongle *first, t_dongle *secnd,
		long cooldown)
{
	pthread_mutex_lock(&first->mtx);
	while (heap_peek(first) != coder->id || get_time_ms()
		- first->released_at < cooldown)
	{
		pthread_mutex_unlock(&first->mtx);
		usleep(1000);
		pthread_mutex_lock(&first->mtx);
	}
	pthread_mutex_lock(&secnd->mtx);
	while (heap_peek(secnd) != coder->id || get_time_ms()
		- secnd->released_at < cooldown)
	{
		pthread_mutex_unlock(&secnd->mtx);
		usleep(1000);
		pthread_mutex_lock(&secnd->mtx);
	}
}

void	fifo_wait_for_turn(t_coder *coder, t_dongle *first, t_dongle *secnd,
	long cooldown)
{
	pthread_mutex_lock(&first->mtx);
	while (first->queue[first->head] != coder->id || get_time_ms()
		- first->released_at < cooldown)
	{
		pthread_mutex_unlock(&first->mtx);
		usleep(1000);
		pthread_mutex_lock(&first->mtx);
	}
	pthread_mutex_lock(&secnd->mtx);
	while (secnd->queue[secnd->head] != coder->id || get_time_ms()
		- secnd->released_at < cooldown)
	{
		pthread_mutex_unlock(&secnd->mtx);
		usleep(1000);
		pthread_mutex_lock(&secnd->mtx);
	}
}

void	acquire_dongles_edf(t_coder *coder, t_dongle *first, t_dongle *secnd,
		long cooldown)
{
	edf_enqueue(coder, first, secnd);
	edf_wait_for_turn(coder, first, secnd, cooldown);
}

void	release_dongles_fifo(t_coder *coder, t_dongle *first, t_dongle *secnd)
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

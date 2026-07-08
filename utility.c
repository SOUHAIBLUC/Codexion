/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utility.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: so-ait-l <so-ait-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 10:46:41 by so-ait-l          #+#    #+#             */
/*   Updated: 2026/05/04 09:03:10 by so-ait-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdint.h>

int	get_over(t_sim *sim)
{
	int	over;

	pthread_mutex_lock(&sim->sim_mtx);
	over = sim->simulation_over;
	pthread_mutex_unlock(&sim->sim_mtx);
	return (over);
}

void	*ft_calloc(size_t count, size_t size)
{
	void	*mem;

	if (count == 0 || size == 0)
		return (malloc(0));
	if (SIZE_MAX / count < size)
		return (NULL);
	mem = malloc(count * size);
	if (!mem)
		return (NULL);
	memset(mem, 0, count * size);
	return (mem);
}

void	smart_usleep(long ms, t_sim *sim)
{
	long	end;

	end = get_time_ms() + ms;
	while (get_time_ms() < end)
	{
		if (get_over(sim))
			return ;
		usleep(1000);
	}
}

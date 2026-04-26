/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utility.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: so-ait-l <so-ait-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 10:46:41 by so-ait-l          #+#    #+#             */
/*   Updated: 2026/04/26 11:02:02 by so-ait-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	get_over(t_sim *sim)
{
	int	over;

	pthread_mutex_lock(&sim->sim_mtx);
	over = sim->simulation_over;
	pthread_mutex_unlock(&sim->sim_mtx);
	return (over);
}

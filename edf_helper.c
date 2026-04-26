/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   edf_helper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: so-ait-l <so-ait-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 09:37:24 by so-ait-l          #+#    #+#             */
/*   Updated: 2026/04/26 10:10:39 by so-ait-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	heap_insert(t_dongle *d, int coder_id, long deadline)
{
	int				i;
	int				parent;
	t_heap_entry	tmp;

	d->heap[d->heap_size].coder_id = coder_id;
	d->heap[d->heap_size].deadline = deadline;
	d->heap_size++;
	i = d->heap_size - 1;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (d->heap[i].deadline < d->heap[parent].deadline)
		{
			tmp = d->heap[i];
			d->heap[i] = d->heap[parent];
			d->heap[parent] = tmp;
			i = parent;
		}
		else
			break ;
	}
}

static void	bubble_down(t_dongle *d)
{
	int				i;
	int				child;
	t_heap_entry	tmp;

	i = 0;
	while (1)
	{
		child = 2 * i + 1;
		if (child >= d->heap_size)
			break ;
		if (child + 1 < d->heap_size
			&& d->heap[child + 1].deadline < d->heap[child].deadline)
			child++;
		if (d->heap[i].deadline <= d->heap[child].deadline)
			break ;
		tmp = d->heap[i];
		d->heap[i] = d->heap[child];
		d->heap[child] = tmp;
		i = child;
	}
}

void	heap_remove(t_dongle *d)
{
	d->heap[0] = d->heap[--d->heap_size];
	bubble_down(d);
}

int	heap_peek(t_dongle *d)
{
	return (d->heap[0].coder_id);
}

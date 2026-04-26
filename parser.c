/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: so-ait-l <so-ait-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 09:38:03 by so-ait-l          #+#    #+#             */
/*   Updated: 2026/04/26 10:54:29 by so-ait-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	validate_chars(char **av)
{
	int	y;
	int	i;

	y = 1;
	while (y <= 7)
	{
		i = 0;
		if (!av[y] || av[y][0] == '\0')
			return (fprintf(stderr, "Error: empty argument %d\n", y), 1);
		while (av[y][i])
		{
			if (av[y][i] < '0' || av[y][i] > '9')
				return (fprintf(stderr, "Error: numeric arguments only\n"), 1);
			i++;
		}
		y++;
	}
	return (0);
}

static void	assign_args(char **av, t_sim *sim)
{
	sim->num_coders = atoi(av[1]);
	sim->time_to_burnout = atoi(av[2]);
	sim->time_to_compile = atoi(av[3]);
	sim->time_to_debug = atoi(av[4]);
	sim->time_to_refactor = atoi(av[5]);
	sim->compiles_required = atoi(av[6]);
	sim->dongle_cooldown = atoi(av[7]);
	sim->scheduler = av[8];
}

static int	validate_values(t_sim *sim)
{
	if (sim->num_coders < 2)
		return (fprintf(stderr, "Error: should be at least 2\n"), 1);
	if (sim->time_to_burnout <= 0 || sim->time_to_compile <= 0
		|| sim->time_to_debug <= 0 || sim->time_to_refactor <= 0
		|| sim->compiles_required <= 0 || sim->dongle_cooldown <= 0)
		return (fprintf(stderr, "Error: wrong value\n"), 1);
	if (strcmp(sim->scheduler, "fifo") != 0
		&& strcmp(sim->scheduler, "edf") != 0)
		return (fprintf(stderr, "Error: wrong scheduler\n"), 1);
	return (0);
}

int	parse_args(int ac, char **av, t_sim *sim)
{
	if (ac != 9)
		return (fprintf(stderr, "Error: wrong number of arguments\n"), 1);
	if (validate_chars(av))
		return (1);
	assign_args(av, sim);
	return (validate_values(sim));
}

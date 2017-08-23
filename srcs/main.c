/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hivian <hivian@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/11 09:43:00 by hivian            #+#    #+#             */
/*   Updated: 2017/08/22 11:48:28 by hivian           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

int				main(int ac, char **av)
{
	t_env		*e;

	if (!(e = (t_env *)malloc(sizeof(t_env))))
		print_error("Error: malloc");
	if (ac != 2)
		print_error("Usage: woody_woodpacker [file]");
	av++;
	run(*av, e);
	free(e);
	return (0);
}

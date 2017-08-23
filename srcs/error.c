/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hivian <hivian@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/05 09:58:36 by hivian            #+#    #+#             */
/*   Updated: 2017/08/14 12:39:49 by hivian           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

void			print_error(char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}

static bool		is_elf(t_env *e)
{
	if (!strncmp((char*)e->ehdr64->e_ident, "\177ELF", 4))
		return (true);
	return (false);
}

void			check_elf64(t_env *e)
{
	if (!is_elf(e))
		print_error("Not an elf file");
	switch(e->ehdr64->e_ident[EI_CLASS])
	{
		case ELFCLASS64:
			break;
		default:
			print_error("Error: File architecture not supported. x86_64 only");
			break;
	}
}

void			check_ehdr(t_env *e)
{
	if (e->ehdr64->e_shentsize != sizeof(Elf64_Shdr))
		print_error("Error: e_shentsize field not matching the size of an ELF shdr");
	if (e->ehdr64->e_phentsize != sizeof(Elf64_Phdr))
		print_error("Error: e_phentsize field not matching the size of an ELF phdr");
}

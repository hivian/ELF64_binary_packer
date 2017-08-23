/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utility.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hivian <hivian@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/11 15:48:57 by hivian            #+#    #+#             */
/*   Updated: 2017/08/22 14:20:21 by hivian           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

Elf64_Shdr		*find_section(char *name, t_env *e)
{
	e->shdr64 = (Elf64_Shdr *)(e->ptr + e->ehdr64->e_shoff);
	e->sh_strtab = &e->shdr64[e->ehdr64->e_shstrndx];
	unsigned char *str_sect = e->ptr + e->sh_strtab->sh_offset;
	char *sname;

	for (int i = 0; i < e->ehdr64->e_shnum; i++) {
		sname = (char *) (str_sect + e->shdr64[i].sh_name);
		if (!strcmp(sname, name))
			return (&e->shdr64[i]);
	}
	return (NULL);
}

void			print_section(char *data, int len)
{
	for (int i = 0; i < len; i++) {
		if (i % 4 == 0 && i != 0)
			printf(" ");
		if (i % 16 == 0 && i != 0)
			printf("\n");
		printf("%02x", (unsigned char)data[i]);
	}
	printf("\n");
}

char			*keyrev(const char *s, int len)
{
	char		*str;
	int			i;

	i = 0;
	if (!(str = (char *)malloc(sizeof(char) * len)))
		return (NULL);
	while (len)
		str[i++] = s[--len];
	return (str);
}

void			free_all(t_env *e)
{
	free(e->new_shdr);
	for (int i = 0; i < e->ehdr64->e_shnum - 1; i++)
		free(e->data_sec[i]);
	free(e->data_sec);
}

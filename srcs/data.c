
#include "elf64_packer.h"

void		copy_data_sec(t_env *e)
{
	if (!(e->data_sec = malloc(sizeof(unsigned char *) * (e->ehdr64->e_shnum))))
		print_error("Error: malloc");
	bzero(e->data_sec, sizeof(unsigned char *) * e->ehdr64->e_shnum);
	for (int i = 0; i < e->ehdr64->e_shnum; i++) {
		if (!(e->data_sec[i] = malloc(e->shdr64[i].sh_size)))
			print_error("Error: malloc");
		bzero(e->data_sec[i], e->shdr64[i].sh_size);
		if (e->shdr64[i].sh_type == SHT_NOBITS)
			e->data_sec[i] = (void *)0;
		else
			memcpy(e->data_sec[i], e->ptr + e->shdr64[i].sh_offset, e->shdr64[i].sh_size);
	}
}


#include "elf64_packer.h"

void		rewrite_phdr(t_env *e)
{
	int		padding;

	e->phdr64 = (Elf64_Phdr *)(e->ptr + e->ehdr64->e_phoff);
	e->last_load = NULL;
	for (int i = 0; i < e->ehdr64->e_phnum; i++) {
		if (e->phdr64[i].p_offset > (unsigned long)e->file_size)
			print_error("Error: file corrupted");
		if (e->phdr64[i].p_type == PT_LOAD) {
			if (!e->last_load) {
				e->first_load = &e->phdr64[i];
				e->last_load = &e->phdr64[i];
			} else if (e->last_load->p_offset <= e->phdr64[i].p_offset) {
				e->last_load = &e->phdr64[i];
				padding = e->last_load->p_memsz - e->last_load->p_filesz;
				e->last_load->p_memsz += e->payload_len;
				e->last_load->p_filesz = e->last_load->p_memsz;
			}
			e->last_load->p_flags = PF_R|PF_W|PF_X;
		}
	}
	if (!e->last_load)
		print_error("Error: no load segment");
	if (e->first_load->p_vaddr == 0)
		print_error("Error: entrypoint memory address is an offset.");
}

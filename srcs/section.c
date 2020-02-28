
#include "elf64_packer.h"

static Elf64_Shdr		add_custom_header(t_env *e)
{
	Elf64_Shdr sh;

	sh.sh_name = 0;
	sh.sh_type = SHT_PROGBITS;
	sh.sh_flags = SHF_ALLOC|SHF_EXECINSTR;
	sh.sh_addr = e->bss_shdr->sh_addr + e->bss_shdr->sh_size;
	sh.sh_offset = e->bss_shdr->sh_addr + e->bss_shdr->sh_size \
		- (e->first_load->p_vaddr + e->first_load->p_align);
	sh.sh_size = e->payload_len;
	sh.sh_link = 0;
	sh.sh_info = 0;
	sh.sh_addralign = 16;
	sh.sh_entsize = 0;

	return (sh);
}

void					rewrite_shdr(t_env *e)
{
	e->shdr64 = (Elf64_Shdr *)(e->ptr + e->ehdr64->e_shoff);
	e->sh_strtab = &e->shdr64[e->ehdr64->e_shstrndx];
	e->new_shdr = malloc(e->ehdr64->e_shentsize * (e->ehdr64->e_shnum + 1));
	unsigned char *str_sect = e->ptr + e->sh_strtab->sh_offset;
	char *sname;
	bool is_bss = false;
	bool first_sec_after_bss = false;

	int i = -1;
	if (e->sh_strtab->sh_offset > (unsigned long)e->file_size || \
	e->sh_strtab->sh_offset == 0)
		print_error("Error: file corrupted");
	if (!(e->new_shdr = malloc(e->ehdr64->e_shentsize * (e->ehdr64->e_shnum + 1))))
		print_error("Error: malloc");
	while (++i < e->ehdr64->e_shnum) {
		if (e->shdr64[i].sh_offset > (unsigned long)e->file_size)
			print_error("Error: file corrupted");
		e->new_shdr[i] = e->shdr64[i];
		sname = (char *) (str_sect + e->shdr64[i].sh_name);
		if (is_bss == true) {
			if (first_sec_after_bss == false) {
				e->new_shdr[i].sh_offset = e->off_custom_sec + e->payload_len;
				first_sec_after_bss = true;
			} else {
				e->new_shdr[i].sh_offset = e->new_shdr[i - 1].sh_offset \
					+ e->new_shdr[i - 1].sh_size;
			}
		} else if (!strcmp(sname, ".bss")) {
			is_bss = true;
			e->bss_shdr = &e->shdr64[i];
			e->off_custom_sec = e->bss_shdr->sh_addr + e->bss_shdr->sh_size \
				- (e->first_load->p_vaddr + e->first_load->p_align);
			e->span_off_bss = e->off_custom_sec - e->bss_shdr->sh_size \
				- e->bss_shdr->sh_offset;
			e->index_custom_sec = i + 1;
		}
	}
	e->new_shdr[i] = add_custom_header(e);
	e->ehdr64->e_entry = e->new_shdr[i].sh_addr;
}

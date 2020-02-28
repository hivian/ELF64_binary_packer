
#ifndef ELF64_PACKER_H
# define ELF64_PACKER_H

# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <sys/mman.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <stdbool.h>
# include <stdlib.h>
# include <elf.h>
# include <time.h>

#define ADDR_LEN 8
#define DATA_LEN ADDR_LEN * 3
#define BINARY_NAME "new_binary"

typedef struct				s_env
{
	void					*ptr;
	Elf64_Ehdr				*ehdr64;
	Elf64_Phdr				*phdr64;
	Elf64_Phdr				*first_load;
	Elf64_Phdr				*last_load;
	Elf64_Shdr				*shdr64;
	Elf64_Shdr				*new_shdr;
	Elf64_Shdr				*bss_shdr;
	Elf64_Shdr				*text_shdr;
	Elf64_Shdr				*sh_strtab;
	char					*keygen;
	char					*payload;
	int						off_custom_sec;
	int						fd_src;
	int						fd_dst;
	int						file_size;
	int						payload_len;
	bool					is_arg;
	int						original_entry_point;
	int						index_custom_sec;
	int						span_off_bss;
	unsigned char			**data_sec;
}							t_env;

void						elf64_packer(t_env *e);
void						check_elf64(t_env *e);
void						check_ehdr(t_env *e);
void						run(char *arg, t_env *e);
void						encrypt_section(t_env *e);
void						rewrite_shdr(t_env *e);
void						rewrite_phdr(t_env *e);
void						copy_data_sec(t_env *e);
void						print_error(char *msg);
Elf64_Shdr					*find_section(char *name, t_env *e);
void						print_section(char *data, int len);
char						*keyrev(const char *s, int len);
void						free_all(t_env *e);

#endif

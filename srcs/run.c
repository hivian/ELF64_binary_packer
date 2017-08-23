/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hivian <hivian@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/04 16:06:06 by hivian            #+#    #+#             */
/*   Updated: 2017/08/23 11:34:25 by hivian           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

char write_and_xor[] = {
	// push
	0x9c, 0x50, 0x57, 0x56, 0x54, 0x52, 0x51,
	// write
	0xbf, 0x01, 0, 0, 0,
	0x48, 0x8d, 0x35, 0x0c, 0, 0, 0,
	0xba, 0x10, 0, 0, 0,
	0x48, 0x89, 0xf8,
	0x0f, 0x05,
	0xeb, 0x10,
	// string
	0x5b, 0x55, 0x6e, 0x70, 0x61, 0x63, 0x6b, 0x69, 0x6e, 0x67, 0x2e, 0x2e, 0x2e, 0x5d, 0x0a, 0,
	/* xor decryption */
	// set the offset: 0x33 = 51 bytes
	0x48, 0x8b, 0x05, 0x33, 0x00, 0x00, 0x00,
	// set the size: 0x34 = 52 bytes
	0x48, 0x8b, 0x0d, 0x34, 0x00, 0x00, 0x00,
	// set the key: 0x1d = 29 bytes
	0x48, 0x8b, 0x15, 0x1d, 0x00, 0x00, 0x00,
	// add    %rax,%rcx
	0x48, 0x01, 0xc1,
	// xor    %dl,(%rax)
	0x30, 0x10,
	// ror    $0x8,%rdx
	0x48, 0xc1, 0xca, 0x08,
	// inc    %rax
	0x48, 0xff, 0xc0,
	// cmp    %rcx,%rax
	0x48, 0x39, 0xc8,
	// jne: 0xf2 = -14 bytes
	0x75, 0xf2,
	// pop
	0x59, 0x5a, 0x5c, 0x5e, 0x5f, 0x58, 0x9d
};

//load the original entry point
unsigned char loader[5];
//load data
unsigned char key[ADDR_LEN];
unsigned char addr[ADDR_LEN];
unsigned char size[ADDR_LEN];

static void		fill_data(t_env *e)
{
	char *key_rev;

	if (!(key_rev = keyrev(e->keygen, ADDR_LEN)))
		print_error("Error: keyrev");
	memcpy(key, key_rev, sizeof(key));
	free(e->keygen);
	free(key_rev);
	unsigned long text_sect_addr = e->text_shdr->sh_addr;
	unsigned long text_sect_size = e->text_shdr->sh_size;
	for (int i = 0; i < ADDR_LEN; i++) {
		addr[i] = text_sect_addr;
		size[i] = text_sect_size;
		text_sect_addr >>= 8;
		text_sect_size >>= 8;
	}
}

static void		fill_loader(t_env *e)
{
	unsigned long off_jump = 0xffffffff - e->off_custom_sec + e->original_entry_point
		- sizeof(write_and_xor) - sizeof(loader) - \
		(e->first_load->p_vaddr + e->first_load->p_align) + 1;
 	// jump
 	loader[0] = 0xe9;
	loader[1] = off_jump;
	loader[2] = off_jump >> 8;
	loader[3] = off_jump >> 16;
	loader[4] = off_jump >> 24;
}

static int file_offset = 0;

static void		write_off(int fd, void *data, int size) {
	if (write(fd, data, size) != size)
		print_error("Error: write");
	file_offset += size;
}

static void		write_pad(int fd, int size) {
	while (file_offset < size)
		write_off(fd, "\0", 1);
}

static void		write_file(t_env *e)
{
	fill_loader(e);
	fill_data(e);
	e->ehdr64->e_shoff = e->new_shdr[e->ehdr64->e_shnum - 1].sh_offset +
		e->new_shdr[e->ehdr64->e_shnum - 1].sh_size;
	write_off(e->fd_dst, e->ehdr64, sizeof(Elf64_Ehdr));
	write_pad(e->fd_dst, e->ehdr64->e_phoff);
	write_off(e->fd_dst, e->phdr64, sizeof(Elf64_Phdr) * e->ehdr64->e_phnum);
	for (int i = 0; i < e->index_custom_sec; i++) {
		if (e->shdr64[i].sh_type != SHT_NOBITS && e->shdr64[i].sh_type != SHT_NULL) {
			write_pad(e->fd_dst, e->new_shdr[i].sh_offset);
			write_off(e->fd_dst, e->data_sec[i], e->new_shdr[i].sh_size);
		}
	}
	write_pad(e->fd_dst, e->bss_shdr->sh_offset + e->bss_shdr->sh_size + e->span_off_bss);
	write_off(e->fd_dst, write_and_xor, sizeof(write_and_xor));
	write_off(e->fd_dst, loader, sizeof(loader));
	write_off(e->fd_dst, key, sizeof(key));
	write_off(e->fd_dst, addr, sizeof(addr));
	write_off(e->fd_dst, size, sizeof(size));
	for (int i = e->index_custom_sec; i < e->ehdr64->e_shnum; i++) {
		write_pad(e->fd_dst, e->new_shdr[i].sh_offset);
		write_off(e->fd_dst, e->data_sec[i], e->new_shdr[i].sh_size);
	}
	write_pad(e->fd_dst, e->ehdr64->e_shoff);
	write_off(e->fd_dst, e->new_shdr, e->ehdr64->e_shentsize * e->ehdr64->e_shnum);
}

void			run(char *arg, t_env *e)
{
	if ((e->fd_src = open(arg, O_RDONLY)) < 0)
		print_error("Error: open");
	if ((e->file_size = lseek(e->fd_src, 0, SEEK_END)) < 0)
		print_error("Error: lseek");
	if ((e->ptr = mmap(0, e->file_size, PROT_READ | PROT_WRITE, \
	MAP_PRIVATE, e->fd_src, 0)) == MAP_FAILED)
		print_error("Error: mmap");
	e->ehdr64 = (Elf64_Ehdr *)e->ptr;
	e->payload_len = sizeof(write_and_xor) + sizeof(loader) + DATA_LEN;
	e->original_entry_point = e->ehdr64->e_entry;
	check_elf64(e);
	check_ehdr(e);
	rewrite_phdr(e);
	rewrite_shdr(e);
	encrypt_section(e);
	copy_data_sec(e);
	if ((e->fd_dst = open(BINARY_NAME, O_CREAT | O_WRONLY | O_TRUNC, 0755)) < 0)
		print_error("Error: open");
	write_file(e);
	free_all(e);
	if (munmap(e->ptr, e->file_size) < 0)
		print_error("Error: munmap");
	close(e->fd_src);
	close(e->fd_dst);
}

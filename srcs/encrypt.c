
#include "elf64_packer.h"

static char		*gen_64_key()
{
	unsigned char key[ADDR_LEN];
	char chars[36] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
	char *res;

	srand(time(NULL));
	bzero(key, sizeof(key));
	for (unsigned int i = 0; i < sizeof(key); i++)
		key[i] = chars[rand() % sizeof(chars)];
	res = strdup((const char *)key);
	return (res);
}

void			xor_section(char *key, unsigned char *data, int len)
{
	int			key_index = ADDR_LEN - 1;

	for (int i = 0; i < len; i++) {
		data[i] ^= key[key_index % 8];
		key_index--;
		if (key_index < 0)
			key_index = ADDR_LEN - 1;
	}
}

void		encrypt_section(t_env *e)
{

	if ((e->text_shdr = find_section(".text", e)) != NULL) {
		if (!(e->keygen = gen_64_key()))
			print_error("Error: gen_64_key");
		xor_section(e->keygen, e->ptr + e->text_shdr->sh_offset, e->text_shdr->sh_size);
		printf("Key = %.8s\n", e->keygen);
  	} else {
		print_error("Error: no .text section");
	}
}

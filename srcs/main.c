
#include "elf64_packer.h"

int				main(int ac, char **av)
{
	t_env		*e;

	if (!(e = (t_env *)malloc(sizeof(t_env))))
		print_error("Error: malloc");
	if (ac != 2)
		print_error("Usage: ./packer [file]");
	av++;
	run(*av, e);
	free(e);
	return (0);
}

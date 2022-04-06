#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <libft.h>

#include <ft_nm.h>

// TODO: Use 'a.out' as default objfile argument
// TODO: Handle big-endian encoding
// TODO: Handle 32-bit files (using unions?)
int	main(int ac, char **av)
{
	int			i;
	int			ret;
	t_elf_err	err;

	i = 1;
	ret = 0;
	err = 0;
	while (ret != -1 && i < ac)
	{
		err = ft_nm(av[0], av[i]);
		if (err == ELF_ESYS)
			ret = -1;
		else if (err != ELF_EOK && err != ELF_ENOSYMS)
			ret += 1;
		i++;
	}
	if (ret == -1)
		ret = -ret;
	return (ret);
}

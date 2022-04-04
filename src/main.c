#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <libft.h>

#include <elf_dump.h>

static int	elf_perror(const char *prog, const char *file, t_elf_err err)
{
	int ret;

	ret = 0;
	if (err == ELF_ESYS)
		perror(prog);
	else if (err != ELF_EOK)
	{
		if (err == ELF_ENOSYMS)
			ft_dprintf(STDERR_FILENO, "%s: %s: no symbols\n", prog, file);
		else
		{
			if (err == ELF_EBADFMT)
				ft_dprintf(STDERR_FILENO, "%s: %s: file format not recognized\n", prog, file);
			else if (err == ELF_EISDIR)
				ft_dprintf(STDERR_FILENO, "%s: Warning: '%s' is a directory\n", prog, file);
			ret = 1;
		}
	}
	return ret;
}

// TODO: Handle ar archives
// TODO: Use 'a.out' as default objfile argument
// TODO: Handle big-endian encoding
// TODO: Handle 32-bit files (using unions?)
int	main(int ac, char **av)
{
	int			i;
	int			ret;
	t_elf_err	err;
	int			fd;

	i = 1;
	ret = 0;
	err = 0;
	while (ret != -1 && i < ac)
	{
		if ((fd = open(av[i], O_RDONLY)) == -1)
		{
			perror("open");
			err = ELF_ESYS;
			ret++;
		}
		else
		{
			err = elf_dump(fd);
			if (err != ELF_EOK)
				ret += elf_perror(av[0], av[i], err);
			if (close(fd) != 0)
			{
				perror("close");
				ret = -1;
			}
		}
		i++;
	}
	return (ret);
}

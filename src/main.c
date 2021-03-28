#include <sys/fcntl.h>

#include <elf_dump.h>

#include <file_map.h>

int	main(int ac, char **av)
{
	int		i;
	int		err;
	int		fd;

	i = 1;
	err = 0;
	while (!err && i < ac)
	{
		if ((fd = open(av[i], O_RDONLY)) == -1)
		{
			perror("open");
			err = 1;
		}
		else
		{
			err = elf_dump(fd);
			close(fd);
		}
		i++;
	}
	return (err);
}

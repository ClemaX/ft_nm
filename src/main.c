#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <elf_dump.h>

// TODO: Use 'a.out' as default objfile argument
// TODO: Handle big-endian encoding
// TODO: Handle 32-bit files (using unions?)
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
			if (err != 0)
				perror("elf_dump");
			if (close(fd) != 0)
			{
				perror("close");
				if (err == 0)
					err = 1;
			}
		}
		i++;
	}
	return (err);
}

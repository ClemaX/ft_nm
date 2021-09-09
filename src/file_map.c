#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

#include <file_map.h>

// TODO: ft_perror

off_t	file_map(int fd, void**addr)
{
	struct stat	st;

	if (fstat(fd, &st) == -1)
	{
		perror("fstat");
		return (-1);
	}
	*addr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (*addr == (void*)-1)
	{
		perror("mmap");
		*addr = NULL;
		return (-1);
	}
	return (st.st_size);
}

int		file_unmap(void **addr, off_t size)
{
	const int	err = munmap(*addr, size);

	if (err == -1)
		perror ("munmap");
	*addr = NULL;
	return (err);
}

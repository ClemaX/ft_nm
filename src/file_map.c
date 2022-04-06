#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <file_map.h>

// TODO: ft_perror

/**
 * @brief	Load a file's content into memory.
 * 
 * @param	fd		File handle.
 * @param	addr	Destination pointer for the file's content.
 * @return	off_t	File size or negative error code.
 */
off_t	file_map(int fd, void **addr)
{
	struct stat	st;
	off_t		ret;

	if (fstat(fd, &st) == -1)
	{
		//perror("fstat");
		ret = -1;
	}
	else
	{
		if (S_ISDIR(st.st_mode))
			ret = FILE_EISDIR;
		else if (!S_ISREG(st.st_mode))
			ret = FILE_EIRREG;
		else
		{
			*addr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
			if (*addr == (void*)-1)
			{
				//perror("mmap");
				*addr = NULL;
				ret = FILE_ESYS;
			}
			else
				ret = st.st_size;
		}
	}
	return (ret);
}

/**
 * @brief	Release a loaded file's memory.
 * 
 * @param	addr	Pointer to the file's content.
 * @param	size	File size.
 * @return	int		Zero or negative error code.
 */
int		file_unmap(void **addr, off_t size)
{
	const int	err = munmap(*addr, size);

	//if (err == -1)
	//	perror ("munmap");
	*addr = NULL;
	return (err);
}

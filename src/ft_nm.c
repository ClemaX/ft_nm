#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <libft.h>

#include <ft_nm.h>
#include <file_map.h>
#include <elf_dump.h>
#include <ar_dump.h>

int	ft_nm_perror(const char *prog, const char *file, t_elf_err err)
{
	int ret;

	ret = 0;
	if (err != ELF_EOK)
	{
		if (err == ELF_ENOSYMS)
			ft_dprintf(STDERR_FILENO, "%s: %s: no symbols\n", prog, file);
		else
		{
			if (err == ELF_EBADFMT)
				ft_dprintf(STDERR_FILENO,
					"%s: %s: file format not recognized\n", prog, file);
			else if (err == ELF_EISDIR)
				ft_dprintf(STDERR_FILENO,
					"%s: Warning: '%s' is a directory\n", prog, file);
			else if (err == ELF_EBADAR)
				ft_dprintf(STDERR_FILENO,
					"%s: %s: malformed archive\n", prog, file);
			ret = 1;
		}
	}
	return ret;
}

int	ft_nm(const char *prog, const char *filename)
{
	void		*data;
	off_t		size;
	int			fd;
	t_elf_err	err;

	if (filename == NULL)
		filename = FT_NM_DEFAULT;
	
	if ((fd = open(filename, O_RDONLY)) != -1)
	{
		size = file_map(fd, &data);
		if (size == FILE_ESYS)
			err = ELF_ESYS;
		else if (size == FILE_EISDIR)
			err = ELF_EISDIR;
		else if (size == FILE_EIRREG)
			err = ELF_EIRREG;
		else
		{
			if (ar_ident(data, (unsigned long)size))
				err = ar_dump(prog, data, (unsigned long)size) * ELF_EBADAR;
			else
				err = elf_dump(data, (unsigned long)size, NULL);
			if (file_unmap(&data, size))
				err = ELF_ESYS;
		}
		if (close(fd) != 0)
			err = ELF_ESYS;
	}
	else
	{
		if (errno == ENOENT)
			ft_dprintf(STDERR_FILENO,
				"%s: '%s': No such file\n", prog, filename);
		else if (errno == EACCES)
			ft_dprintf(STDERR_FILENO,
				"%s: %s: Permission denied\n", prog, filename);
		err = ELF_ESYS;
	}
	if (err != ELF_EOK)
		ft_nm_perror(prog, filename, err);
	return (err);
}
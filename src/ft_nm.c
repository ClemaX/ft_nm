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
	const char	*messages[6] = {
		[ELF_EOK]=NULL,
		[ELF_EBADFMT]="%s: %s: file format not recognized\n",
		[ELF_EISDIR]="%s: Warning: '%s' is a directory\n",
		[ELF_EBADAR]="%s: %s: malformed archive\n",
		[ELF_ENOSYMS]="%s: %s: no symbols\n",
	};
	int ret;

	ret = 0;
	if (err != ELF_EOK && err != ELF_ESYS)
	{
		ft_dprintf(STDERR_FILENO, messages[err], prog, file);
		ret = err != ELF_ENOSYMS;
	}
	return ret;
}

int			ft_nm(const char *filename, t_elf_opt options, const char *prog)
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
				err = ar_dump(data, (unsigned long)size, options, prog) * ELF_EBADAR;
			else
				err = elf_dump(data, (unsigned long)size, options, NULL);
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
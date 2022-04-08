#include <stdlib.h>
#include <stdint.h>
#include <ar.h>

#include <libft.h>

#include <ar_iter.h>
#include <ar_dump.h>
#include <elf_dump.h>

#include <ft_nm.h>

int 		ar_ident(const void *data, unsigned long size)
{
	return (size > SARMAG && *(uint64_t*)data == *(uint64_t*)ARMAG
		&& ft_strncmp(ARFMAG,
			((const struct ar_hdr *)(data + SARMAG))->ar_fmag,
				sizeof(ARFMAG) - 1) == 0);
}

static void	ar_dump_member(const char *name, unsigned long name_len,
	const void *data, unsigned long size, t_elf_opt options, const char *prog)
{
	t_elf_err	err;
	char		*name_dup;

	name_dup = ft_strndup(name, name_len);
	if (name_dup != NULL)
	{
		err = elf_dump(data, size, options, name_dup);
		if (err != ELF_EOK)
			ft_nm_perror((const char*)prog, name_dup, err);
		free(name_dup);
	}
}
int	        ar_dump(const void *data, unsigned long size, t_elf_opt options,
	const char *prog)
{
	t_ar_map	map;
	int			err;
	
	err = ar_map(&map, data, size);
	if (err == 0)
	{
		ar_iter(&map, ar_dump_member, options, prog);
		ar_unmap(&map);
	}
	return (err);
}
#include <sys/types.h>
#include <stdint.h>
#include <elf.h>

#include <libft.h>

#include <file_map.h>
#include <elf_dump.h>
#include <elf_map.h>
#include <elf_sym.h>

static uint8_t		elf_ident(const void *data, unsigned long size)
{
	const unsigned char			*e_ident = (unsigned char *)data;
	uint8_t						elf_class;

	elf_class = ELFCLASSNONE;
	if (size > EI_NIDENT && *(uint32_t*)e_ident == *((uint32_t*)ELFMAG))
		elf_class = e_ident[EI_CLASS];
	return (elf_class);
}

static t_elf_err	elf_dump_syms_64(const t_elf_map_64 *map)
{
	t_list		*symbols;
	t_elf_err	err;

	err = elf_load_syms_64(&symbols, map);

	if (err == 0)
	{
		// TODO: Handle comparison by address ('-n/-v/--numeric-sort')
		// TODO: Handle no sort ('-p/--no-sort')
		// TODO: Handle reverse sort ('-r/--reverse-sort')
		ft_lstsort(&symbols, &elf_sym_cmp);
		ft_lstiter(symbols, &elf_print_sym);
		ft_lstclear(&symbols, NULL);
	}
	return (err);
}

static t_elf_err	elf_dump_64(const void *data, unsigned long size)
{
	t_elf_map_64	map;
	t_elf_err		err;

	err = elf_map_64(&map, data, size);
	if (err == 0)
	{
		if (map.sym_count == 0)
			err = ELF_ENOSYMS;
		else
			err = elf_dump_syms_64(&map);
	}
	return (err);
}


static t_elf_err	elf_dump_syms_32(const t_elf_map_32 *map)
{
	t_list		*symbols;
	t_elf_err	err;

	err = elf_load_syms_32(&symbols, map);

	if (err == 0)
	{
		// TODO: Handle comparison by address ('-n/-v/--numeric-sort')
		// TODO: Handle no sort ('-p/--no-sort')
		// TODO: Handle reverse sort ('-r/--reverse-sort')
		ft_lstsort(&symbols, &elf_sym_cmp);
		ft_lstiter(symbols, &elf_print_sym);
		ft_lstclear(&symbols, NULL);
	}
	return (err);
}

static t_elf_err	elf_dump_32(const void *data, unsigned long size)
{
	t_elf_map_32	map;
	t_elf_err		err;

	err = elf_map_32(&map, data, size);
	if (err == 0)
	{
		if (map.sym_count == 0)
			err = ELF_ENOSYMS;
		else
			err = elf_dump_syms_32(&map);
	}
	return (err);
}

/**
 * @brief	Dump an opened ELF file.
 * 
 * @param	fd			The opened file's handle.
 * @return	t_elf_err	Zero or error code.
 */
t_elf_err		elf_dump(int fd)
{
	void		*data;
	const off_t	size = file_map(fd, &data);
	t_elf_err	err;
	int			elf_class;

	if (size == FILE_ESYS)
		err = ELF_ESYS;
	else if (size == FILE_EISDIR)
		err = ELF_EISDIR;
	else if (size == FILE_EIRREG)
		err = ELF_EIRREG;
	else
	{
		elf_class = elf_ident(data, (unsigned long)size);
		if (elf_class == ELFCLASS32)
			err = elf_dump_32(data, (unsigned long)size);
		else if (elf_class == ELFCLASS64)
			err = elf_dump_64(data, (unsigned long)size);
		else
			err = ELF_EBADFMT;
		if (file_unmap(&data, size))
			err = ELF_ESYS;
	}
	return (err);
}

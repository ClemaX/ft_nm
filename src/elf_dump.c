#include <sys/types.h>
#include <stdint.h>
#include <elf.h>

#include <libft.h>

#include <file_map.h>
#include <elf_dump.h>
#include <elf_map.h>
#include <elf_sym.h>

static uint8_t	elf_ident(const void *data, unsigned long size)
{
	const unsigned char			*e_ident = (unsigned char *)data;
	uint8_t						elf_class;

	elf_class = ELFCLASSNONE;
	if (size > EI_NIDENT
	&& *(uint32_t*)e_ident == *((uint32_t*)ELFMAG))
		elf_class = e_ident[EI_CLASS];
	return (elf_class);
}

static int		elf_dump_32(void const *data, unsigned long size)
{
	int	err;

	err = 0;
	(void)data;
	(void)size;
	return (err);
}

static int		elf_dump_syms_64(const t_elf_map_64 *map)
{
	t_list	*symbols;
	int		ret;

	ret = elf_load_syms_64(&symbols, map);

	if (ret == 0)
	{
		// TODO: Handle comparison by address ('-n/-v/--numeric-sort')
		// TODO: Handle no sort ('-p/--no-sort')
		// TODO: Handle reverse sort ('-r/--reverse-sort')
		ft_lstsort(&symbols, &elf_sym_cmp_64);
		ft_lstiter(symbols, &elf_print_sym_64);
		ft_lstclear(&symbols, NULL);
	}
	return (0);
}

static int		elf_dump_64(const void *data, unsigned long size)
{
	t_elf_map_64	map;
	int				err;

	err = elf_map_64(&map, data, size);
	if (err == 0)
	{
		if (map.sym_count == 0)
			ft_printf("no symbols\n");
		else
			err = elf_dump_syms_64(&map);
	}
	return (err);
}

/**
 * @brief	Dump an opened ELF file.
 * 
 * @param	fd	The opened file's handle.
 * @return	int	Zero or error code.
 */
int				elf_dump(int fd)
{
	void		*data;
	const off_t	size = file_map(fd, &data);
	int			err;
	int			elf_class;

	if (size == (off_t)-1)
		return ((int)size);

	//ft_printf("Data size: "PRIuOFF_T" bytes\n", size);

	elf_class = elf_ident(data, (unsigned long)size);
	//ft_printf("File class: %d\n", elf_class);

	if (elf_class == ELFCLASS32)
		err = elf_dump_32(data, (unsigned long)size);
	else if (elf_class == ELFCLASS64)
		err = elf_dump_64(data, (unsigned long)size);
	else
	{
		err = ELF_EBADFMT;
		ft_dprintf(2, "Unknown file class!\n");
	}
	if (file_unmap(&data, size) && !err)
		err = -1;
	return (err);
}

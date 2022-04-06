#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <elf.h>

#include <libft.h>

#include <file_map.h>
#include <ar_iter.h>
#include <elf_dump.h>
#include <elf_map.h>
#include <elf_sym.h>

static uint8_t	elf_ident(const void *data, unsigned long size)
{
	const unsigned char			*e_ident = (unsigned char *)data;
	uint8_t						elf_class;

	elf_class = ELFCLASSNONE;
	if (size > EI_NIDENT && *(uint32_t*)e_ident == *((uint32_t*)ELFMAG))
		elf_class = e_ident[EI_CLASS];
	return (elf_class);
}

// TODO: Handle comparison by address ('-n/-v/--numeric-sort')
// TODO: Handle no sort ('-p/--no-sort')
// TODO: Handle reverse sort ('-r/--reverse-sort')

t_elf_err		elf_dump(const void *data, unsigned long size, const char *name)
{
	const uint8_t	elf_class = elf_ident(data, size);
	t_elf_map_64	map;
	t_elf_funs		funs[2] = (t_elf_funs[]){
		{
			(t_elf_map_fun*)elf_map_32,
			(t_elf_unmap_fun*)elf_unmap_32,
			(t_elf_load_fun*)elf_load_syms_32,
			(t_elf_print_fun*)elf_print_sym_32
		},
		{
			(t_elf_map_fun*)elf_map_64,
			(t_elf_unmap_fun*)elf_unmap_64,
			(t_elf_load_fun*)elf_load_syms_64,
			(t_elf_print_fun*)elf_print_sym_64
		},
	};
	t_list			*symbols;
	t_elf_err		err;

	if (elf_class != ELFCLASSNONE)
	{
		if (elf_class <= sizeof(funs) / sizeof(*funs))
		{
			err = funs[elf_class - 1].map(&map, data, size);
			if (err == 0)
			{
				err = funs[elf_class - 1].load(&symbols, &map);
				if (err == 0)
				{
					if (name != NULL)
						ft_dprintf(STDERR_FILENO, "\n%s:\n", name);
					ft_lstsort(&symbols, elf_sym_cmp);
					ft_lstiter(symbols, funs[elf_class - 1].print);
					ft_lstclear(&symbols, NULL);
				}
				funs[elf_class - 1].unmap(&map);
			}
		}
		else
		{
			if (name != NULL)
				ft_dprintf(STDERR_FILENO, "\n%s:\n", name);
			err = ELF_ENOSYMS;
		}
	}
	else
		err = ELF_EBADFMT;
	return (err);
}
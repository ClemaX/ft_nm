#ifndef ELF_DUMP_H
# define ELF_DUMP_H

#include <elf_def.h>
#include <libft.h>

typedef t_elf_err	(t_elf_map_fun(void *map, const void *data, unsigned long size));
typedef void		(t_elf_unmap_fun(void *map));

typedef t_elf_err	(t_elf_load_fun(t_list **symbols, const void *map,
	t_elf_opt options));
typedef void		(t_elf_print_fun(void *symbol));

typedef struct	s_elf_funs
{
	t_elf_map_fun	*map;
	t_elf_unmap_fun	*unmap;
	t_elf_load_fun	*load;
	t_elf_print_fun	*print;
}				t_elf_funs;

t_elf_err	elf_dump(const void *data, unsigned long size, t_elf_opt options,
	const char *prog);

#endif

#ifndef ELF_SYM_H
# define ELF_SYM_H

# include <libft.h>

# include <elf_map.h>

typedef struct	s_elf_sym_64
{
	const Elf64_Sym	*symbol;
	const char		*name;
	char			type;
}				t_elf_sym_64;

char	elf_sym_locate(const t_elf_map_64 *map, const Elf64_Sym *symbol);

char	elf_sym_type_64(const t_elf_map_64 *map, const Elf64_Sym *symbol);

t_list	*elf_load_sym_64(const t_elf_map_64 *map, const Elf64_Sym *symbol);

t_list	*elf_load_syms_64(const t_elf_map_64 *map);

int		elf_sym_cmp_64(void *a, void *b);

void	elf_print_sym_64(void *data);

#endif

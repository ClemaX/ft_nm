#ifndef ELF_SYM_H
# define ELF_SYM_H

# include <libft.h>

# include <elf_map.h>

# define ELF_SYMID_UNKNOWN ELF_SHID_UNKNOWN
// Undefined symbol
# define ELF_SYMID_UNDEFINED 'U'
// Unique global symbol
# define ELF_SYMID_UNIQUE 'u'
// Weakly bound symbol
# define ELF_SYMID_WEAK 'w'

typedef struct	s_elf_sym_64
{
	const Elf64_Sym	*symbol;
	const char		*name;
	char			identifier;
}				t_elf_sym_64;

char	elf_sym_locate(const t_elf_map_64 *map, const Elf64_Sym *symbol);

char	elf_sym_type_64(const t_elf_map_64 *map, const Elf64_Sym *symbol);

t_list	*elf_load_sym_64(const t_elf_map_64 *map, const Elf64_Sym *symbol);

int		elf_load_syms_64(t_list **dest, const t_elf_map_64 *map);

int		elf_sym_cmp_64(void *a, void *b);

void	elf_print_sym_64(void *data);

#endif

#ifndef ELF_SYM_H
# define ELF_SYM_H

# include <libft.h>

# include <elf_def.h>

// Unknown symbol
# define ELF_SYMID_UNKNOWN ELF_SHID_UNKNOWN
// Undefined symbol
# define ELF_SYMID_UNDEFINED 'U'
// Unique global symbol
# define ELF_SYMID_UNIQUE 'u'
// Weakly bound object symbol
# define ELF_SYMID_WEAKOBJ 'v'
// Weakly bound symbol
# define ELF_SYMID_WEAK 'w'
// Indirect object symbol
# define ELF_SYMID_INDIRECT 'i'

typedef struct	s_elf_sym
{
	const void		*symbol;
	Elf64_Addr		value;
	const char		*name;
	char			identifier;
}				t_elf_sym;

char	elf_sym_locate(const t_elf_map_64 *map, const Elf64_Sym *symbol);

char	elf_sym_type_64(const t_elf_map_64 *map, const Elf64_Sym *symbol);
char	elf_sym_type_32(const t_elf_map_32 *map, const Elf32_Sym *symbol);

t_list	*elf_load_sym_64(const t_elf_map_64 *map, const Elf64_Sym *symbol);
t_list	*elf_load_sym_32(const t_elf_map_32 *map, const Elf32_Sym *symbol);

int		elf_load_syms_64(t_list **dest, const t_elf_map_64 *map);
int		elf_load_syms_32(t_list **dest, const t_elf_map_32 *map);

int		elf_sym_cmp(void *a, void *b);

void	elf_print_sym(void *data);

#endif

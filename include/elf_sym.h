#ifndef ELF_SYM_H
# define ELF_SYM_H

# include <libft.h>

# include <elf_def.h>

// Unknown symbol
# define ELF_SYMID_UNKNOWN ELF_SHID_UNKNOWN
// Undefined symbol
# define ELF_SYMID_UNDEFINED 'U'
// Absolute symbol
# define ELF_SYMID_ABS 'a'
// Common symbol
# define ELF_SYMID_COMMON 'c'
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

# define elf_sym_isextern(symbol)\
	(ELF32_ST_BIND(symbol->st_info) == STB_GLOBAL\
		|| ELF32_ST_BIND(symbol->st_info) == STB_WEAK)

# define elf_sym_isdebug(symbol)\
	(ELF32_ST_TYPE(symbol->st_info) == STT_FILE)

# define elf_sym_isnamed(map, symbol)\
	(*(map->str + symbol->st_name) != '\0')

# define elf_sym_isundef(symbol)\
	(symbol->st_shndx == SHN_UNDEF)

char	elf_sym_locate(const t_elf_map_64 *map, const Elf64_Sym *symbol);

t_list	*elf_sym_load_64(const t_elf_map_64 *map, const Elf64_Sym *symbol);
t_list	*elf_sym_load_32(const t_elf_map_32 *map, const Elf32_Sym *symbol);

int		elf_syms_load_64(t_list **dest, const t_elf_map_64 *map,
	t_elf_opt options);
int		elf_syms_load_32(t_list **dest, const t_elf_map_32 *map,
	t_elf_opt options);

int		elf_sym_cmp(void *a, void *b);

void	elf_sym_print_64(const Elf64_Sym *data);
void	elf_sym_print_32(const Elf32_Sym *data);

#endif

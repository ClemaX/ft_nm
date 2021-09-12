#ifndef ELF_MAP_H
# define ELF_MAP_H

# include <elf.h>

# define ELF_EBADFMT 1

# define ELF_SHID_UNKNOWN '?'
# define ELF_SHID_STRTAB 1
# define ELF_SHID_SYMTAB 2
# define ELF_SHID_ABS 'a'
# define ELF_SHID_COMMON 'c'
# define ELF_SHID_BSS 'b'
# define ELF_SHID_DATA 'd'
# define ELF_SHID_READONLY 'r'
# define ELF_SHID_SDATA 's'
# define ELF_SHID_TEXT 't'
# define ELF_SHID_STAB '-'

typedef struct	s_elf_map_64
{
	const Elf64_Ehdr	*eh;
	const Elf64_Phdr	*ph;
	const Elf64_Shdr	*sh;
	char				*shid;
	const char			*str;
	const char			*shstr;
	const Elf64_Sym		*sym;
	Elf64_Xword			sym_count;
}				t_elf_map_64;

typedef struct	s_elf_map_32
{
	const Elf32_Ehdr	*eh;
	const Elf32_Phdr	*ph;
	const Elf32_Shdr	*sh;
	const Elf64_Sym		*sym;
	const char			*str;
	const char			*shstr;
}				t_elf_map_32;

// This struct is used as a predicate to match sections
typedef struct	s_elf_section_hint
{
	// Type is matched using == operation
	unsigned int	type;
	// Flags are matched using &                operation
	Elf64_Xword		flags;
	// This pointer can be null to match any section
	const char		*name;
	// Identifier to set
	char		identifier;
}				t_elf_section_hint;

int	elf_map_64(t_elf_map_64 *map, const void *data, unsigned long size);

const char	*elf_shstr_64(const t_elf_map_64 *map, Elf64_Xword str_offset);

#endif

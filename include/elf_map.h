#ifndef ELF_MAP_H
# define ELF_MAP_H

# define ELF_EBADFMT 1

#include <elf.h>

typedef struct	s_elf_map_64
{
	const Elf64_Ehdr	*eh;
	const Elf64_Phdr	*ph;
	const Elf64_Shdr	*sh;
	const char			*str;
	const char			*shstr;
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

int	elf_map_64(t_elf_map_64 *map, const void *data, unsigned long size);

const char	*elf_shstr_64(const t_elf_map_64 *map, Elf64_Xword str_offset);

#endif

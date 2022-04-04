#ifndef ELF_MAP_H
# define ELF_MAP_H

#include <elf_def.h>

int			elf_map_64(t_elf_map_64 *map, const void *data, unsigned long size);

const char	*elf_shstr_64(const t_elf_map_64 *map, Elf64_Xword str_offset);

#endif

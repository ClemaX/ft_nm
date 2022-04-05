#ifndef ELF_MAP_H
# define ELF_MAP_H

#include <elf_def.h>

char	    elf_map_shid(const char *name, unsigned int type, Elf64_Xword flags);

const char	*elf_shstr_64(const t_elf_map_64 *map, Elf64_Xword str_offset);
const char	*elf_shstr_32(const t_elf_map_32 *map, Elf32_Xword str_offset);

int			elf_map_64(t_elf_map_64 *map, const void *data, unsigned long size);
int			elf_map_32(t_elf_map_32 *map, const void *data, unsigned long size);

#endif

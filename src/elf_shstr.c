#include <elf_map.h>
#include <stdlib.h>

const char	*elf_shstr_64(const t_elf_map_64 *map, Elf64_Xword str_offset)
{
	if (map->shstr && str_offset < map->sh[map->eh->e_shstrndx].sh_size)
		return map->shstr + str_offset;
	return NULL;
}

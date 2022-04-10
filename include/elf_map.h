#ifndef ELF_MAP_H
# define ELF_MAP_H

#include <elf_def.h>

/**
 * @brief	Get a pointer to a section header string, given it's offset.
 * 
 * @param	map			Parsed ELF map.
 * @param	str_offset	String offset.
 * @return	const char*	Pointer to the string, or NULL if out of bounds.
 */
# define    elf_shstr(map, str_offset)\
    (map->shstr && str_offset < map->sh[map->eh->e_shstrndx].sh_size\
        ? map->shstr + str_offset\
        : NULL)

/**
 * @brief	Get a pointer to a symbol string, given it's offset.
 * 
 * @param	map			Parsed ELF map.
 * @param	str_offset	String offset.
 * @return	const char*	Pointer to the string, or NULL if out of bounds.
 */
/* # define    elf_str(map, str_offset)\
    (map->str && str_offset < map->sh[map->strndx].sh_size\
        ? map->str + str_offset\
        : NULL)

 */

char		elf_map_shid(const char *name, unsigned int type, Elf64_Xword flags);

const char	*elf_shstr_64(const t_elf_map_64 *map, Elf64_Xword str_offset);
const char	*elf_shstr_32(const t_elf_map_32 *map, Elf32_Xword str_offset);

int			elf_map_64(t_elf_map_64 *map, const void *data, unsigned long size);
int			elf_map_32(t_elf_map_32 *map, const void *data, unsigned long size);

void		elf_unmap_64(t_elf_map_64 *map);
void		elf_unmap_32(t_elf_map_32 *map);

#endif

#include <libft.h>
#include <elf_map.h>

#include <stdlib.h>
#include <unistd.h>

/**
 * @brief	Map special ELF sections and check for duplicates.
 * 
 * @param	dest	The destination for the section's data pointer.
 * @param	data	The section's data pointer.
 * @param	name	The section's name, for error messages.
 * @return	int		Zero or error code.
 */
static int	elf_map_section(const void **dest, const void *data, const char *name)
{
	int	ret;

	if (*dest != NULL)
	{
		ft_dprintf(STDERR_FILENO, "Multiple '%s' sections!\n", name);
		ret = ELF_EBADFMT;
	}
	else
	{
		*dest = data;
		ret = 0;
	}
	return ret;
}

/**
 * @brief	Map 64 bit ELF sections names and identifiers from object data.
 * 
 * @param	map		Destination for mapped data.
 * @param	data	64 bit ELF object data.
 * @return	int		Zero or error code.
 */
static int	elf_map_sections_64(t_elf_map_64 *map, const void *data)
{
	int		i;
	int		ret;

	ret = 0;
	if (map->eh->e_shnum != 0)
	{
		map->shid = malloc(sizeof(*map->shid) * map->eh->e_shnum);
		if (map->shid != NULL)
		{
			i = 0;
			while (ret == 0 && i < map->eh->e_shnum)
			{
				map->shid[i] = elf_map_shid(map->shstr + map->sh[i].sh_name,
					map->sh[i].sh_type, map->sh[i].sh_flags);
				if (map->shid[i] == ELF_SHID_STRTAB)
					ret = elf_map_section((const void**)&map->str, data + map->sh[i].sh_offset,
						map->shstr + map->sh[i].sh_name);
				else if (map->shid[i] == ELF_SHID_SYMTAB)
				{
					ret = elf_map_section((const void**)&map->sym, data + map->sh[i].sh_offset,
						map->shstr + map->sh[i].sh_name);
					if (ret == 0)
						map->sym_count = map->sh[i].sh_size / map->sh[i].sh_entsize;
				}
				i++;
			}
		}
		else
		{
			ret = -1;
			ft_dprintf(2, "symid[%u]: allocation failed\n", map->eh->e_shnum);
		}
	}
	else
		map->shid = NULL;
	return ret;
}

/**
 * @brief	Map 32 bit ELF sections names and identifiers from object data.
 * 
 * @param	map		Destination for mapped data.
 * @param	data	32 bit ELF object data.
 * @return	int		Zero or error code.
 */
static int	elf_map_sections_32(t_elf_map_32 *map, const void *data)
{
	int		i;
	int		ret;

	ret = 0;
	if (map->eh->e_shnum != 0)
	{
		map->shid = malloc(sizeof(*map->shid) * map->eh->e_shnum);
		if (map->shid != NULL)
		{
			i = 0;
			while (ret == 0 && i < map->eh->e_shnum)
			{
				map->shid[i] = elf_map_shid(map->shstr + map->sh[i].sh_name,
					map->sh[i].sh_type, map->sh[i].sh_flags);
				if (map->shid[i] == ELF_SHID_STRTAB)
					ret = elf_map_section((const void**)&map->str, data + map->sh[i].sh_offset,
						map->shstr + map->sh[i].sh_name);
				else if (map->shid[i] == ELF_SHID_SYMTAB)
				{
					ret = elf_map_section((const void**)&map->sym, data + map->sh[i].sh_offset,
						map->shstr + map->sh[i].sh_name);
					if (ret == 0)
						map->sym_count = map->sh[i].sh_size / map->sh[i].sh_entsize;
				}
				i++;
			}
		}
		else
		{
			ret = -1;
			ft_dprintf(2, "symid[%u]: allocation failed\n", map->eh->e_shnum);
		}
	}
	else
		map->shid = NULL;
	return ret;
}

/**
 * @brief	Map a 64 bit ELF object's headers and sections.
 * 
 * @param	map		Destination for mapped data.
 * @param	data	64 bit ELF object data.
 * @param	size	ELF object data size.
 * @return	int		Zero or error code.
 */
int	elf_map_64(t_elf_map_64 *map, const void *data, unsigned long size)
{
	int ret;

	map->eh = (Elf64_Ehdr*)data;

	if (size < sizeof(Elf64_Ehdr)
	|| size < map->eh->e_phoff + map->eh->e_phentsize * map->eh->e_phnum
	|| size < map->eh->e_shoff + map->eh->e_shentsize * map->eh->e_shnum)
	{
		ft_dprintf(STDERR_FILENO, "Invalid ELF header!\n");
		ret = ELF_EBADFMT;
	}
	else
	{
		map->ph = (Elf64_Phdr *)((unsigned char*)data + map->eh->e_phoff);
		map->sh = (Elf64_Shdr *)((unsigned char*)data + map->eh->e_shoff);
		if (map->eh->e_shstrndx != SHN_UNDEF)
			map->shstr = (const char *)data + map->sh[map->eh->e_shstrndx].sh_offset;
		else
			map->shstr = (const char *)NULL;
		map->str = NULL;
		map->sym = NULL;
		map->sym_count = 0;

		ret = elf_map_sections_64(map, data);
	}
	return (ret);
}

/**
 * @brief	Map a 32 bit ELF object's headers and sections.
 * 
 * @param	map		Destination for mapped data.
 * @param	data	32 bit ELF object data.
 * @param	size	ELF object data size.
 * @return	int		Zero or error code.
 */
int	elf_map_32(t_elf_map_32 *map, const void *data, unsigned long size)
{
	int ret;

	map->eh = (Elf32_Ehdr*)data;

	if (size < sizeof(Elf32_Ehdr)
	|| size < map->eh->e_phoff + map->eh->e_phentsize * map->eh->e_phnum
	|| size < map->eh->e_shoff + map->eh->e_shentsize * map->eh->e_shnum)
	{
		ft_dprintf(STDERR_FILENO, "Invalid ELF header!\n");
		ret = ELF_EBADFMT;
	}
	else
	{
		map->ph = (Elf32_Phdr *)((unsigned char*)data + map->eh->e_phoff);
		map->sh = (Elf32_Shdr *)((unsigned char*)data + map->eh->e_shoff);
		if (map->eh->e_shstrndx != SHN_UNDEF)
			map->shstr = (const char *)data + map->sh[map->eh->e_shstrndx].sh_offset;
		else
			map->shstr = (const char *)NULL;
		map->str = NULL;
		map->sym = NULL;
		map->sym_count = 0;

		ret = elf_map_sections_32(map, data);
	}
	return (ret);
}
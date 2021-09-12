#include <libft.h>
#include <elf_map.h>

#include <stdlib.h>

int		elf_map_shcmp(const t_elf_section_hint *hint, const char *name,
	unsigned int type, unsigned int flags)
{
	return (!(hint->type == type && (hint->flags & flags) == flags
		&& (hint->name == NULL || ft_strcmp(hint->name, name) == 0)));
}

char	elf_map_shid(const char *name, unsigned int type, unsigned int flags)
{
	static const t_elf_section_hint	hints[] = {
		{SHT_STRTAB, SHF_ALLOC, ".strtab", ELF_SHID_STRTAB},
		{SHT_SYMTAB, SHF_ALLOC, ".symtab", ELF_SHID_SYMTAB},
		{SHT_NOBITS, SHF_ALLOC | SHF_WRITE, ".bss", ELF_SHID_BSS},
		{SHT_PROGBITS, SHF_ALLOC | SHF_WRITE, ".data", ELF_SHID_DATA},
	};
	unsigned char	i;
	char			identifier;

	//ft_printf("Identifying '%s'...", name);
	i = 0;
	while (i != sizeof(hints) / sizeof(*hints)
	&& elf_map_shcmp(hints + i, name, type, flags) != 0)
		i++;
	if (i == sizeof(hints) / sizeof(*hints))
		identifier = ELF_SHID_UNKNOWN;
	else
		identifier = hints[i].identifier;
	//ft_printf(" %c\n", identifier);
	return identifier;
}

int	elf_map_section(const void **dest, const void *data, const char *name)
{
	int	ret;

	if (*dest != NULL)
	{
		ft_printf("Multiple '%s' sections!\n", name);
		ret = ELF_EBADFMT;
	}
	else
	{
		*dest = data;
		ret = 0;
	}
	return ret;
}
/*
typedef struct	s_elf_map_shid
{
	void	**dest;
	char	identifier;
}				t_elf_map_shid


	const t_elf_map_shid	map[] = {
		{map->str, ELF_SHID_STRTAB}
	};
*/

int	elf_map_sections_64(t_elf_map_64 *map, const void *data)
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
			ret = -1;
	}
	else
		map->shid = NULL;
	return ret;
}

int	elf_map_64(t_elf_map_64 *map, const void *data, unsigned long size)
{
	map->eh = (Elf64_Ehdr*)data;
/*
	ft_printf("Program header table offset: %lu\n", map->eh->e_phoff);
	ft_printf("Program header entry count: %hu\n", map->eh->e_phnum);
	ft_printf("Section header table offset: %lu\n", map->eh->e_shoff);
	ft_printf("Section header string table index: %hu\n", map->eh->e_shstrndx);
*/
	if (size < sizeof(Elf64_Ehdr)
	|| size < map->eh->e_phoff + map->eh->e_phentsize * map->eh->e_phnum
	|| size < map->eh->e_shoff + map->eh->e_shentsize * map->eh->e_shnum)
		return (ELF_EBADFMT);
	map->ph = (Elf64_Phdr *)((unsigned char*)data + map->eh->e_phoff);
	map->sh = (Elf64_Shdr *)((unsigned char*)data + map->eh->e_shoff);
	if (map->eh->e_shstrndx != SHN_UNDEF)
		map->shstr = (const char *)data + map->sh[map->eh->e_shstrndx].sh_offset;
	else
		map->shstr = (const char *)NULL;
	map->str = NULL;
	map->sym = NULL;
	map->sym_count = 0;
	return (elf_map_sections_64(map, data));
}

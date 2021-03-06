#include <libft.h>
#include <elf_map.h>

#include <stdlib.h>

int		elf_map_shcmp(const t_elf_section_hint *hint, const char *name,
	unsigned int type, Elf64_Xword flags)
{
	//ft_printf("%s: Hint flags: %llu, flags: %llu, match: %d\n", name, hint->flags, flags, (hint->flags & flags) == hint->flags);
	return (!((hint->type == 0 || hint->type == type)
		&& (hint->flags & flags) == hint->flags
		&& (hint->name == NULL || ft_strcmp(hint->name, name) == 0)));
}

char	elf_map_shid(const char *name, unsigned int type, Elf64_Xword flags)
{
	static const t_elf_section_hint	hints[] = {
		{SHT_NOBITS, SHF_ALLOC | SHF_WRITE, ".bss", ELF_SHID_BSS},
		{SHT_STRTAB, 0, ".strtab", ELF_SHID_STRTAB},
		{SHT_SYMTAB, 0, ".symtab", ELF_SHID_SYMTAB},
		{SHT_PROGBITS, 0, ".stab", ELF_SHID_STAB},
		{SHT_PROGBITS, SHF_ALLOC | SHF_EXECINSTR, NULL, ELF_SHID_TEXT},
		{0, SHF_ALLOC | SHF_WRITE | SHF_IA_64_SHORT, NULL, ELF_SHID_SDATA},
		{0, SHF_ALLOC | SHF_WRITE, NULL, ELF_SHID_DATA},
		{0, SHF_ALLOC, NULL, ELF_SHID_READONLY},
	};
	unsigned char	i;
	char			identifier;

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
		ft_dprintf(2, "Multiple '%s' sections!\n", name);
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
		{
			ret = -1;
			ft_dprintf(2, "symid[%u]: allocation failed\n", map->eh->e_shnum);
		}
	}
	else
		map->shid = NULL;
	return ret;
}

int	elf_map_64(t_elf_map_64 *map, const void *data, unsigned long size)
{
	map->eh = (Elf64_Ehdr*)data;

	if (size < sizeof(Elf64_Ehdr)
	|| size < map->eh->e_phoff + map->eh->e_phentsize * map->eh->e_phnum
	|| size < map->eh->e_shoff + map->eh->e_shentsize * map->eh->e_shnum)
	{
		ft_dprintf(2, "Program header table offset: "PRIuOFF_T"\n", map->eh->e_phoff);
		ft_dprintf(2, "Program header entry count: %hu\n", map->eh->e_phnum);
		ft_dprintf(2, "Section header table offset: "PRIuOFF_T"\n", map->eh->e_shoff);
		ft_dprintf(2, "Section header string table index: %hu\n", map->eh->e_shstrndx);

		return (ELF_EBADFMT);
	}
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

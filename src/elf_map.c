#include <libft.h>
#include <elf_map.h>

#include <stdlib.h>

int	elf_map_64(t_elf_map_64 *map, const void *data, unsigned long size)
{
	Elf64_Half	i;

	map->eh = (Elf64_Ehdr*)data;

	if (size < sizeof(Elf64_Ehdr)
	|| size < map->eh->e_phoff + map->eh->e_phentsize * map->eh->e_phnum
	|| size < map->eh->e_shoff + map->eh->e_shentsize * map->eh->e_shnum)
		return (ELF_EBADFMT);
/*
	ft_printf("Program header table offset: %lu\n", map->eh->e_phoff);
	ft_printf("Program header entry count: %hu\n", map->eh->e_phnum);
	ft_printf("Section header table offset: %lu\n", map->eh->e_shoff);
	ft_printf("Section header string table index: %hu\n", map->eh->e_shstrndx); */

	map->ph = (Elf64_Phdr *)((unsigned char*)data + map->eh->e_phoff);
	map->sh = (Elf64_Shdr *)((unsigned char*)data + map->eh->e_shoff);
	map->shstr = (const char *)data + map->sh[map->eh->e_shstrndx].sh_offset;
	map->str = NULL;

	i = 0;
	while (i < map->eh->e_shnum)
	{
		if (map->sh[i].sh_type == SHT_STRTAB
		&& !ft_strncmp(".strtab", map->shstr + map->sh[i].sh_name, 8))
		{
			ft_printf("%02d: %04lx, %s, %02x\n", i, map->sh[i].sh_offset,
				map->shstr + map->sh[i].sh_name, map->sh[i].sh_type);
			if (map->str)
			{
				ft_printf("Multiple strtabs!\n");
				return (ELF_EBADFMT);
			}
			map->str = (const char *)data + map->sh[i].sh_offset;
		}
		/* else
		{
			ft_printf("%02d: %04lx, %s, %02x\n", i, map->sh[i].sh_offset,
				map->shstr + map->sh[i].sh_name, map->sh[i].sh_type);
		} */
		i++;
	}
	return (0);
}

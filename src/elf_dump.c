#include <ft_elf.h>
#include <elf_dump.h>
#include <file_map.h>

#include <elf.h>

#include <stdio.h>
#include <sys/fcntl.h>

#include <libft.h>

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


int	elf_identify(const void *data, unsigned long size)
{
	const struct s_ident *const	e_ident = (struct s_ident*)data; // TODO: Maybe use char array with defined indicies
	int							elf_class;

	elf_class = ELFCLASSNONE;
	if (size > sizeof(struct s_ident)
	&& e_ident->magic_number == *((uint32_t*)ELFMAG))
		elf_class = e_ident->file_class;
	return (elf_class);
}

int	elf_dump_32(void const *data, unsigned long size)
{
	int	err;

	err = 0;
	(void)data;
	(void)size;
	return (err);
}

const char	*elf_shstr_64(const t_elf_map_64 *map, Elf64_Xword str_offset)
{
	if (map->shstr && str_offset < map->sh[map->eh->e_shstrndx].sh_size)
		return map->shstr + str_offset;
	return NULL;
}

int	elf_map_64(t_elf_map_64 *map, const void *data, unsigned long size)
{
	Elf64_Half	i;

	map->eh = (Elf64_Ehdr*)data;

	if (size < sizeof(Elf64_Ehdr)
	|| size < map->eh->e_phoff + map->eh->e_phentsize * map->eh->e_phnum
	|| size < map->eh->e_shoff + map->eh->e_shentsize * map->eh->e_shnum)
		return (ELF_EBADFMT);
/*
	printf("Program header table offset: %lu\n", map->eh->e_phoff);
	printf("Program header entry count: %hu\n", map->eh->e_phnum);
	printf("Section header table offset: %lu\n", map->eh->e_shoff);
	printf("Section header string table index: %hu\n", map->eh->e_shstrndx); */

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
			printf("%02d: %04lx, %s, %02x\n", i, map->sh[i].sh_offset,
				map->shstr + map->sh[i].sh_name, map->sh[i].sh_type);
			if (map->str)
			{
				printf("Multiple strtabs!\n");
				return (ELF_EBADFMT);
			}
			map->str = (const char *)data + map->sh[i].sh_offset;
		}
		/* else
		{
			printf("%02d: %04lx, %s, %02x\n", i, map->sh[i].sh_offset,
				map->shstr + map->sh[i].sh_name, map->sh[i].sh_type);
		} */
		i++;
	}
	return (0);
}

int	elf_dump_syms_64(const t_elf_map_64 *map)
{
	Elf64_Half	i;
	Elf64_Xword	j;
	Elf64_Sym	*sym;
	Elf64_Xword	sym_count;

	i = 0;
	while (i < map->eh->e_shnum)
	{
		if (map->sh[i].sh_type == SHT_SYMTAB)
		{
			sym_count = map->sh[i].sh_size / map->sh[i].sh_entsize;
			sym = (Elf64_Sym*)((unsigned char*)map->eh + map->sh[i].sh_offset);
			printf("Dumping symbols from '%s'[%lu]:\n", elf_shstr_64(map, map->sh[i].sh_name), sym_count);
			j = 0;
			while (j < sym_count)
			{
				printf("%04lx . %s\n", sym[j].st_value, map->str + sym[j].st_name);
				j++;
			}
		}
		i++;
	}
	return (0);
}

int	elf_dump_64(const void *data, unsigned long size)
{
	t_elf_map_64	map;
	int				err;

	err = elf_map_64(&map, data, size);
	if (!err)
		err = elf_dump_syms_64(&map);
	return (err);
}

int	elf_dump(int fd)
{
	void		*data;
	const off_t	size = file_map(fd, &data);
	int			err;
	int			elf_class;

	if (size == (off_t)-1)
		return ((int)size);

	printf("Data size: %lu bytes\n", size);

	elf_class = elf_identify(data, (unsigned long)size);
	printf("File class: %d\n", elf_class);

	if (elf_class == ELFCLASS32)
		err = elf_dump_32(data, (unsigned long)size);
	else if (elf_class == ELFCLASS64)
		err = elf_dump_64(data, (unsigned long)size);
	else
		err = ELF_EBADFMT;
	if (file_unmap(&data, size) && !err)
		err = -1;
	return (err);
}

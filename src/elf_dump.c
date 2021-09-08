#include <ft_elf.h>
#include <elf_dump.h>
#include <file_map.h>

#include <elf.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

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

typedef struct	s_elf_sym_64
{
	const Elf64_Sym	*symbol;
	const char		*name;
}				t_elf_sym_64;


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

/*
typedef struct	s_elf_sym_type_64
{
	Elf64_Word	type;
	char		identifier;
}				t_elf_sym_type_64;

// TODO: Upcase globalq
t_elf_sym_type_64	g_sym_types_64[] = {{STT_NOTYPE, 'a'}, {STT_OBJECT, 'r'}, {STT_FILE, 'a'}};
 */

t_list	*elf_load_sym_64(const t_elf_map_64 *map, const Elf64_Sym *symbol)
{
	t_list			*list_elem;
	t_elf_sym_64	*new_sym;

	list_elem = malloc(sizeof(*list_elem) + sizeof(*new_sym));
	if (!list_elem)
		return (NULL);
	new_sym = (t_elf_sym_64*)(list_elem + 1);
	new_sym->symbol = symbol;
	new_sym->name = map->str + symbol->st_name;
	list_elem->content = new_sym;
	list_elem->next = NULL;
	return (list_elem);
}

t_list	*elf_load_syms_64(const t_elf_map_64 *map)
{
	t_list			*symbols;
	t_list			*elem;
	Elf64_Sym		*sym;
	Elf64_Xword	sym_count;
	Elf64_Half	i;
	Elf64_Xword	j;

	symbols = NULL;
	i = 0;
	while (i < map->eh->e_shnum)
	{
		printf("%u %s\n", map->sh[i].sh_type,
			elf_shstr_64(map, map->sh[i].sh_name));
		if (map->sh[i].sh_type == SHT_SYMTAB)
		{
			sym = (Elf64_Sym*)((unsigned char*)map->eh + map->sh[i].sh_offset);
			sym_count = map->sh[i].sh_size / map->sh[i].sh_entsize;
			printf("Loading symbols from '%s'[%lu]...\n",
				elf_shstr_64(map, map->sh[i].sh_name), sym_count);
			j = 0;
			while (j < sym_count)
			{
				if (*(map->str + sym[j].st_name))
				{
					if (!(elem = elf_load_sym_64(map, sym + j)))
					{
						ft_lstclear(&symbols, NULL);
						return (NULL);
					}
					printf("Loading %s...\n", map->str + sym[j].st_name);
					ft_lstadd_front(&symbols, elem);
				}
				j++;
			}
		}
		i++;
	}
	return (symbols);
}

int	elf_sym_cmp_64(void *a, void *b)
{
	const t_elf_sym_64 *const	sym_a = (t_elf_sym_64*)a;
	const t_elf_sym_64 *const	sym_b = (t_elf_sym_64*)b;
	int							diff;
	int							pref;

	const char	*name_a = sym_a->name;
	const char	*name_b = sym_b->name;

	if (*name_a == '_')
	{
		pref = 1;
		while (*name_a == '_')
			name_a++;
	}
	if (*name_b == '_')
	{
		pref = -1;
		while (*name_b == '_')
			name_b++;
	}
	while (*name_a && ft_tolower(*name_a) == ft_tolower(*name_b))
	{
		name_a++;
		name_b++;
	}
	diff = ft_tolower(*name_b) - ft_tolower(*name_a);
	if (!diff)
		return (pref);
	return (diff);
}

void	elf_print_sym_64(void *data)
{
	const t_elf_sym_64 *const	sym = (t_elf_sym_64*)data;

	if (ELF32_ST_TYPE(sym->symbol->st_info) != STT_FILE)
	{
		printf("%016lx %02u:%02u %s\n",
			sym->symbol->st_value,
			ELF32_ST_BIND(sym->symbol->st_info),
			ELF32_ST_TYPE(sym->symbol->st_info),
			sym->name);
	}

}

#define ELF_SYM_TYPES "aaa"
int	elf_dump_syms_64(const t_elf_map_64 *map)
{
	t_list	*symbols;

	symbols = elf_load_syms_64(map);
	if (!symbols)
		return (1);
	ft_lstsort(&symbols, &elf_sym_cmp_64);
	ft_lstiter(symbols, &elf_print_sym_64);
	ft_lstclear(&symbols, NULL);
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

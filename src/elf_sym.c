#include <stdlib.h>

#include <elf_sym.h>

char	is_weak_64(const t_elf_map_64 *map, const Elf64_Sym *symbol)
{
	char	type = '?';

	(void) map;
	if (ELF64_ST_BIND(symbol->st_info) & STB_WEAK)
		type = 'w';

	return type;
}


char	elf_sym_locate(const t_elf_map_64 *map, const Elf64_Sym *symbol)
{
	char	type = '?';

	(void) map;
	if (symbol->st_shndx == SHN_ABS)
		type = 'a';
	else if (symbol->st_shndx == SHN_COMMON)
		type = 'c';
	// TODO: Handle bss data and text sections
	return type;
}

char	elf_sym_type_64(const t_elf_map_64 *map, const Elf64_Sym *symbol)
{
	char	type;


	type = elf_sym_locate(map, symbol);

	if (symbol->st_shndx == SHN_ABS)
		type = 'A';
	else if (map->sh[symbol->st_shndx].sh_type == SHT_NOBITS && map->sh[symbol->st_shndx].sh_flags == (SHF_ALLOC | SHF_WRITE) && map->sh[symbol->st_shndx].sh_info)
		type = 'B'; // TODO: Get bind and set local bind

	if (ft_isalpha(type) && ELF64_ST_BIND(symbol->st_info) == STB_GLOBAL)
		type += 'A' - 'a';

	return type;
}

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
	//new_sym->type = elf_sym_type_64(map, symbol);
	list_elem->content = new_sym;
	list_elem->next = NULL;
	return (list_elem);
}

t_list	*elf_load_syms_64(const t_elf_map_64 *map)
{
	t_list			*symbols;
	t_list			*elem;
	Elf64_Sym		*sym;
	Elf64_Xword		sym_count;
	Elf64_Half		i;
	Elf64_Xword		j;

	symbols = NULL;
	i = 0;
	while (i < map->eh->e_shnum)
	{
		ft_printf("Found sh %u:%lu %s...\n", map->sh[i].sh_type, map->sh[i].sh_flags,
			elf_shstr_64(map, map->sh[i].sh_name));
		if (map->sh[i].sh_type == SHT_SYMTAB)
		{
			sym = (Elf64_Sym*)((unsigned char*)map->eh + map->sh[i].sh_offset);
			sym_count = map->sh[i].sh_size / map->sh[i].sh_entsize;
			ft_printf("Loading symbols from '%s'[%lu]...\n",
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
					ft_printf("Loading %d->%s...\n", sym[j].st_shndx, map->str + sym[j].st_name);
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
		// TODO: Get the sections's type from shstndx
		ft_printf("%016lx %02u:%02u %s\n",
			sym->symbol->st_value,
			ELF32_ST_BIND(sym->symbol->st_info),
			ELF32_ST_TYPE(sym->symbol->st_info),
			sym->name);
	}

}

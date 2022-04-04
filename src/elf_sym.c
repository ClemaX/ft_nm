#include <stdlib.h>

#include <elf_sym.h>

#include <inttypes.h>

/**
 * @brief	Identify a symbol according to it's location.
 * 
 * @param	map		Parsed ELF map.
 * @param	symbol	ELF symbol.
 * @return	char	Symbol identifier.
 */
char	elf_sym_locate(const t_elf_map_64 *map, const Elf64_Sym *symbol)
{
	char	identifier = ELF_SYMID_UNKNOWN;

	(void) map;
	if (symbol->st_shndx == SHN_ABS)
		identifier = ELF_SHID_ABS;
	else if (symbol->st_shndx == SHN_COMMON)
		identifier = ELF_SHID_COMMON;
	else if (ELF64_ST_BIND(symbol->st_info) & STB_WEAK)
	{
		if (ELF64_ST_TYPE(symbol->st_info) & STT_OBJECT)
			identifier = ELF_SYMID_WEAKOBJ;
		else
			identifier = ELF_SYMID_WEAK;
		if (symbol->st_value != 0)
			identifier += 'A' - 'a';
	}
	else
		identifier = map->shid[symbol->st_shndx];
	// TODO: Handle overflow by using sh_count!
	return identifier;
}

/**
 * @brief	Identify a symbol according to it's location and attributes.
 * 
 * @param	map		Parsed ELF map.
 * @param	symbol	ELF symbol.
 * @return	char	Symbol identifier.	
 */
char	elf_sym_type_64(const t_elf_map_64 *map, const Elf64_Sym *symbol)
{
	char	identifier;

	identifier = elf_sym_locate(map, symbol);
	if (identifier == ELF_SYMID_UNKNOWN && symbol->st_value == 0)
		identifier = ELF_SYMID_UNDEFINED;
	if (ft_islower(identifier) && ELF64_ST_BIND(symbol->st_info) == STB_GLOBAL)
		identifier += 'A' - 'a';
	return identifier;
}

/**
 * @brief	Validate a symbol's section header index.
 * 
 * @param	map		Parsed ELF map.
 * @param	symbol	ELF symbol.
 * @return	int		Zero or error code.
 */
int		elf_sym_validate_64(const t_elf_map_64 *map, const Elf64_Sym *symbol)
{
	if (symbol->st_shndx > map->eh->e_shnum && !(symbol->st_shndx > SHN_LORESERVE && symbol->st_shndx < SHN_HIRESERVE))
	{
		ft_printf("Invalid symbol: section header index(%u) exceeds e_shnum(%u)!\n", symbol->st_shndx, map->eh->e_shnum);
		return ELF_EBADFMT;
	}
	return 0;
}

/**
 * @brief Load and identify a symbol into a list node.
 * 
 * @param	map		Parsed ELF map.
 * @param	symbol	ELF symbol.
 * @return	t_list*	Allocated list node or NULL in case of error.
 */
t_list	*elf_load_sym_64(const t_elf_map_64 *map, const Elf64_Sym *symbol)
{
	t_list			*list_elem;
	t_elf_sym_64	*new_sym;

	list_elem = malloc(sizeof(*list_elem) + sizeof(*new_sym));
	if (list_elem != NULL)
	{	new_sym = (t_elf_sym_64*)(list_elem + 1);
		new_sym->symbol = symbol;
		new_sym->name = map->str + symbol->st_name;
		new_sym->identifier = elf_sym_type_64(map, symbol);
		list_elem->content = new_sym;
		list_elem->next = NULL;
	}
	return (list_elem);
}

/**
 * @brief	Validate, load and identify ELF symbols into a linked-list.
 * 
 * @param	dest	Destination pointer for the head of the linked list.
 * @param	map		Parsed ELF map.
 * @return	int		Zero or error code.
 */
int	elf_load_syms_64(t_list	**dest, const t_elf_map_64 *map)
{
	t_list			*elem;
	Elf64_Half		i;
	int				ret;

	ret = 0;
	*dest = NULL;
	i = 0;
	while (ret == 0 && i < map->sym_count)
	{
		ret = elf_sym_validate_64(map, map->sym + i);
		if (ret == 0 && *(map->str + map->sym[i].st_name))
		{
			if (!(elem = elf_load_sym_64(map, map->sym + i)))
			{
				ft_lstclear(dest, NULL);
				return (-1);
			}
			//ft_printf("Loading %d->%s...\n", map->sym[i].st_shndx, map->str + map->sym[i].st_name);
			ft_lstadd_front(dest, elem);
		}
		i++;
	}
	return (ret);
}

/**
 * @brief	Compare two symbols names.
 * 
 * @param	a	Pointer to symbol a.
 * @param	b	Pointer to symbol b.
 * @return	int	The difference according to lexicographical order.
 */
int	elf_sym_cmp_64(void *a, void *b)
{
	const t_elf_sym_64 *const	sym_a = (t_elf_sym_64*)a;
	const t_elf_sym_64 *const	sym_b = (t_elf_sym_64*)b;
	const char	*name_a = sym_a->name;
	const char	*name_b = sym_b->name;

	while (*name_a != '\0' && *name_a == *name_b)
	{
		name_a++;
		name_b++;
	}
	return (*name_b - *name_a);
}

/*
	TODO:

	If a symbol has version information associated with it,
	then the version information is displayed as well. If the versioned
	symbol is undefined or hidden from linker, the version string is
	displayed as a suffix to the symbol name, preceded by an @ character.
	For example ‘foo@VER_1’.

	If the version is the default version to be used when resolving unversioned
	references to the symbol, then it is displayed as a suffix preceded by two
	@ characters.
	For example ‘foo@@VER_2’.
*/
void	elf_print_sym_64(void *data)
{
	const t_elf_sym_64 *const	sym = (t_elf_sym_64*)data;
	Elf64_Addr					sym_value;

	// TODO: Toggle this predicate using '-A/-o/--print-file-name' flag
	if (ELF32_ST_TYPE(sym->symbol->st_info) != STT_FILE)
	{
		if (sym->identifier == ELF_SYMID_UNDEFINED || sym->identifier == ELF_SYMID_WEAK)
			ft_printf("%16s %c %s\n", "",
				sym->identifier, sym->name);
		else
		{
			if (sym->identifier == ELF_SHID_COMMON
			|| sym->identifier == ELF_SHID_COMMON + ('A' - 'a'))
				sym_value = sym->symbol->st_size;
			else
				sym_value = sym->symbol->st_value;
			ft_printf("%016"PRIx64" %c %s\n",
				sym_value, sym->identifier, sym->name);
		}
	}
}

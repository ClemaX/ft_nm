#include <stdlib.h>

#include <elf_sym.h>

#include <inttypes.h>

/**
 * @brief	Identify a 32 bit symbol according to it's location and attributes.
 * 
 * @param	map		Parsed 32 bit ELF map.
 * @param	symbol	32 bit ELF symbol.
 * @return	char	Symbol identifier.	
 */
static char	elf_sym_type_32(const t_elf_map_32 *map, const Elf32_Sym *symbol)
{
	char	identifier = ELF_SYMID_UNKNOWN;

	if (symbol->st_shndx == SHN_ABS)
		identifier = ELF_SYMID_ABS;
	else if (symbol->st_shndx == SHN_COMMON)
		identifier = ELF_SYMID_COMMON;
	else if (ELF64_ST_BIND(symbol->st_info) == STB_WEAK)
	{
		if (ELF64_ST_TYPE(symbol->st_info) == STT_OBJECT)
			identifier = ELF_SYMID_WEAKOBJ;
		else
			identifier = ELF_SYMID_WEAK;
		if (symbol->st_shndx != SHN_UNDEF)
			identifier += 'A' - 'a';
	}
	else if (symbol->st_shndx == SHN_UNDEF)
		identifier = ELF_SYMID_UNDEFINED;
	else if (ELF64_ST_TYPE(symbol->st_info) == STT_GNU_IFUNC)
		identifier = ELF_SYMID_INDIRECT;
	else if (symbol->st_shndx < map->eh->e_shnum)
		identifier = map->shid[symbol->st_shndx];	
	if (ft_islower(identifier) && ELF64_ST_BIND(symbol->st_info) == STB_GLOBAL)
		identifier += 'A' - 'a';
	return identifier;
}

/**
 * @brief	Identify a 64 bit symbol according to it's location and attributes.
 * 
 * @param	map		Parsed 64 bit ELF map.
 * @param	symbol	64 bit ELF symbol.
 * @return	char	Symbol identifier.	
 */
static char	elf_sym_type_64(const t_elf_map_64 *map, const Elf64_Sym *symbol)
{
	char	identifier = ELF_SYMID_UNKNOWN;

	if (symbol->st_shndx == SHN_ABS)
		identifier = ELF_SYMID_ABS;
	else if (symbol->st_shndx == SHN_COMMON)
		identifier = ELF_SYMID_COMMON;
	else if (ELF32_ST_BIND(symbol->st_info) == STB_WEAK)
	{
		if (ELF32_ST_TYPE(symbol->st_info) == STT_OBJECT)
			identifier = ELF_SYMID_WEAKOBJ;
		else
			identifier = ELF_SYMID_WEAK;
		if (symbol->st_shndx != SHN_UNDEF)
			identifier += 'A' - 'a';
	}
	else if (symbol->st_shndx == SHN_UNDEF)
		identifier = ELF_SYMID_UNDEFINED;
	else if ((ELF32_ST_TYPE(symbol->st_info) == STT_GNU_IFUNC))
		identifier = ELF_SYMID_INDIRECT;
	else
		identifier = map->shid[symbol->st_shndx];	
	if (ft_islower(identifier) && ELF64_ST_BIND(symbol->st_info) == STB_GLOBAL)
		identifier += 'A' - 'a';
	return identifier;
}

/**
 * @brief	Validate a 64 bit ELF symbol's section header index.
 * 
 * @param	map		Parsed 64 bit ELF map.
 * @param	symbol	64 bit ELF symbol.
 * @return	int		Zero or error code.
 */
int			elf_sym_validate_64(const t_elf_map_64 *map, const Elf64_Sym *symbol)
{
	if ((symbol->st_shndx > map->eh->e_shnum
		&& !(symbol->st_shndx > SHN_LORESERVE
			&& symbol->st_shndx < SHN_HIRESERVE))
	|| (symbol->st_name >= map->sh[map->strndx].sh_size))
	{
		ft_printf("Invalid symbol: section header index(%u) exceeds \
e_shnum(%u)!\n", symbol->st_shndx, map->eh->e_shnum);
		return ELF_EBADFMT;
	}
	return 0;
}

/**
 * @brief	Validate a 32 bit ELF symbol's section header index.
 * 
 * @param	map		Parsed 32 bit ELF map.
 * @param	symbol	32 bit ELF symbol.
 * @return	int		Zero or error code.
 */
t_elf_err	elf_sym_validate_32(const t_elf_map_32 *map, const Elf32_Sym *symbol)
{
	if ((symbol->st_shndx > map->eh->e_shnum
		&& !(symbol->st_shndx > SHN_LORESERVE
			&& symbol->st_shndx < SHN_HIRESERVE))
	|| (symbol->st_name >= map->sh[map->strndx].sh_size))
	{
		ft_printf("Invalid symbol: section header index(%u) exceeds \
e_shnum(%u)!\n", symbol->st_shndx, map->eh->e_shnum);
		return ELF_EBADFMT;
	}
	return 0;
}

static int	elf_sym_filter_64(const t_elf_map_64 *map, const Elf64_Sym *symbol, t_elf_opt options)
{
	int	match;

	match = elf_sym_isdebug(symbol) ? (options & ELF_ODEBUG) : elf_sym_isnamed(map, symbol);
	if (match && (options & (ELF_OEXTERN | ELF_OUNDEF)))
		match = elf_sym_isextern(symbol)
			&& (!(options & ELF_OUNDEF) || elf_sym_isundef(symbol));
	return (match);
}

static int	elf_sym_filter_32(const t_elf_map_32 *map, const Elf32_Sym *symbol, t_elf_opt options)
{
	int	match;

	match = elf_sym_isdebug(symbol) ? (options & ELF_ODEBUG) : elf_sym_isnamed(map, symbol);
	if (match && (options & (ELF_OEXTERN | ELF_OUNDEF)))
		match = elf_sym_isextern(symbol)
			&& (!(options & ELF_OUNDEF) || elf_sym_isundef(symbol));
	return (match);
}

/**
 * @brief Load and identify a 64 bit ELF symbol into a list node.
 * 
 * @param	map		Parsed 64 bit ELF map.
 * @param	symbol	64 bit ELF symbol.
 * @return	t_list*	Allocated list node or NULL in case of error.
 */
t_list		*elf_sym_load_64(const t_elf_map_64 *map, const Elf64_Sym *symbol)
{
	t_list		*list_elem;
	t_elf_sym	*new_sym;

	list_elem = malloc(sizeof(*list_elem) + sizeof(*new_sym));
	if (list_elem != NULL)
	{	new_sym = (t_elf_sym*)(list_elem + 1);
		new_sym->symbol = symbol;
		new_sym->name = map->str + symbol->st_name;
		new_sym->identifier = elf_sym_type_64(map, symbol);
		if (new_sym->identifier != ELF_SYMID_COMMON
		&& new_sym->identifier != ELF_SYMID_COMMON + ('A' - 'a'))
			new_sym->value = symbol->st_value;
		else
			new_sym->value = symbol->st_size;
		list_elem->content = new_sym;
		list_elem->next = NULL;
	}
	return (list_elem);
}

/**
 * @brief Load and identify a 32 bit ELF symbol into a list node.
 * 
 * @param	map		Parsed 32 bit ELF map.
 * @param	symbol	32 bit ELF symbol.
 * @return	t_list*	Allocated list node or NULL in case of error.
 */
t_list		*elf_sym_load_32(const t_elf_map_32 *map, const Elf32_Sym *symbol)
{
	t_list		*list_elem;
	t_elf_sym	*new_sym;

	list_elem = malloc(sizeof(*list_elem) + sizeof(*new_sym));
	if (list_elem != NULL)
	{	new_sym = (t_elf_sym*)(list_elem + 1);
		new_sym->symbol = symbol;
		new_sym->name = map->str + symbol->st_name;
		new_sym->identifier = elf_sym_type_32(map, symbol);
		if (new_sym->identifier != ELF_SYMID_COMMON
		&& new_sym->identifier != ELF_SYMID_COMMON + ('A' - 'a'))
			new_sym->value = symbol->st_value;
		else
			new_sym->value = symbol->st_size;
		list_elem->content = new_sym;
		list_elem->next = NULL;
	}
	return (list_elem);
}

/**
 * @brief	Validate, load and identify 64 bit ELF symbols into a linked-list.
 * 
 * @param	dest		Destination pointer for the head of the linked list.
 * @param	map			Parsed 64 bit ELF map.
 * @param	options		ELF symbol filters.	
 * @return	t_elf_err	Zero or error code.
 */
t_elf_err	elf_syms_load_64(t_list	**dest, const t_elf_map_64 *map,
	t_elf_opt options)
{
	t_lstadd_fun	*add;
	t_list			*elem;
	Elf64_Half		i;
	int				err;

	add = (options & ELF_ONOSORT) == 0 ? ft_lstadd_front : ft_lstadd_back;
	err = ELF_EOK;
	*dest = NULL;
	i = 0;
	while (err == ELF_EOK && i < map->sym_count)
	{
		err = elf_sym_validate_64(map, map->sym + i);
		if (err == ELF_EOK)
		{
			if (elf_sym_filter_64(map, map->sym + i, options))
			{
				elem = elf_sym_load_64(map, map->sym + i);
				if (elem != NULL)
					add(dest, elem);
				else
				{
					ft_lstclear(dest, NULL);
					err = ELF_ESYS;
				}
			}
		}
		i++;
	}
	if (err == ELF_EOK && *dest == NULL)
		err = ELF_ENOSYMS;
	return (err);
}

/**
 * @brief	Validate, load and identify 32 bit ELF symbols into a linked-list.
 * 
 * @param	dest		Destination pointer for the head of the linked list.
 * @param	map			Parsed 32 bit ELF map.
 * @param	options		ELF symbol filters.
 * @return	t_elf_err	Zero or error code.
 */
t_elf_err	elf_syms_load_32(t_list	**dest, const t_elf_map_32 *map,
	t_elf_opt options)
{
	t_list			*elem;
	Elf32_Half		i;
	int				err;

	err = ELF_EOK;
	*dest = NULL;
	i = 0;
	while (err == ELF_EOK && i < map->sym_count)
	{
		err = elf_sym_validate_32(map, map->sym + i);
		if (err == ELF_EOK)
		{
			if (elf_sym_filter_32(map, map->sym + i, options))
			{
				elem = elf_sym_load_32(map, map->sym + i);
				if (elem != NULL)
					ft_lstadd_front(dest, elem);
				else
				{
					ft_lstclear(dest, NULL);
					err = ELF_ESYS;
				}
			}
		}
		i++;
	}
	if (err == ELF_EOK && *dest == NULL)
		err = ELF_ENOSYMS;
	return (err);
}

/**
 * @brief	Compare two symbols names.
 * 
 * @param	a	Pointer to symbol a.
 * @param	b	Pointer to symbol b.
 * @return	int	The difference according to lexicographical order.
 */
int			elf_sym_cmp(void *a, void *b)
{
	const t_elf_sym *const	sym_a = (t_elf_sym*)a;
	const t_elf_sym *const	sym_b = (t_elf_sym*)b;
	const char				*name_a = sym_a->name;
	const char				*name_b = sym_b->name;

	while (*name_a != '\0' && *name_a == *name_b)
	{
		name_a++;
		name_b++;
	}
	return (*name_b - *name_a);
}

/**
 * @brief	Print an ELF symbol's value, identifier and name.
 * 
 * @param	data		Pointer to the ELF symbol.
 * @param	value_width	Value field width.
 */
static void	elf_sym_print(const void *data, unsigned value_width)
{
	const t_elf_sym *const	sym = (t_elf_sym*)data;

	// TODO: Filter on symbol loading
	// TODO: Handle variable value size
	if (sym->identifier == ELF_SYMID_UNDEFINED
	|| sym->identifier == ELF_SYMID_WEAK
	|| sym->identifier == ELF_SYMID_WEAKOBJ)
		ft_printf("%*s %c %s\n", value_width,
			"", sym->identifier, sym->name);
	else
		ft_printf("%0*"PRIx64" %c %s\n", value_width,
			sym->value, sym->identifier, sym->name);
}

/**
 * @brief	Print a 64 bit ELF symbol's value, identifier and name.
 * 
 * @param	data		Pointer to the ELF symbol.
 */
void		elf_sym_print_64(const Elf64_Sym *data)
{
	elf_sym_print(data, sizeof(data->st_value) * 2);
}

/**
 * @brief	Print a 32 bit ELF symbol's value, identifier and name.
 * 
 * @param	data		Pointer to the ELF symbol.
 */
void		elf_sym_print_32(const Elf32_Sym *data)
{
	elf_sym_print(data, sizeof(data->st_value) * 2);
}

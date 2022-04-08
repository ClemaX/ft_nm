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
		identifier = ELF_SHID_ABS;
	else if (symbol->st_shndx == SHN_COMMON)
		identifier = ELF_SHID_COMMON;
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
		identifier = ELF_SHID_UNDEFINED;
	else if (ELF64_ST_TYPE(symbol->st_info) == STT_GNU_IFUNC)
		identifier = ELF_SYMID_INDIRECT;
	else
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
		identifier = ELF_SHID_ABS;
	else if (symbol->st_shndx == SHN_COMMON)
		identifier = ELF_SHID_COMMON;
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
		identifier = ELF_SHID_UNDEFINED;
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
	if (symbol->st_shndx > map->eh->e_shnum
	&& !(symbol->st_shndx > SHN_LORESERVE && symbol->st_shndx < SHN_HIRESERVE))
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
	if (symbol->st_shndx > map->eh->e_shnum
	&& !(symbol->st_shndx > SHN_LORESERVE && symbol->st_shndx < SHN_HIRESERVE))
	{
		ft_printf("Invalid symbol: section header index(%u) exceeds \
e_shnum(%u)!\n", symbol->st_shndx, map->eh->e_shnum);
		return ELF_EBADFMT;
	}
	return 0;
}

/**
 * @brief Load and identify a 64 bit ELF symbol into a list node.
 * 
 * @param	map		Parsed 64 bit ELF map.
 * @param	symbol	64 bit ELF symbol.
 * @return	t_list*	Allocated list node or NULL in case of error.
 */
t_list		*elf_load_sym_64(const t_elf_map_64 *map, const Elf64_Sym *symbol)
{
	t_list		*list_elem;
	t_elf_sym	*new_sym;

	list_elem = malloc(sizeof(*list_elem) + sizeof(*new_sym));
	if (list_elem != NULL)
	{	new_sym = (t_elf_sym*)(list_elem + 1);
		new_sym->symbol = symbol;
		new_sym->name = map->str + symbol->st_name;
		new_sym->identifier = elf_sym_type_64(map, symbol);
		if (new_sym->identifier != ELF_SHID_COMMON
		&& new_sym->identifier != ELF_SHID_COMMON + ('A' - 'a'))
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
t_list		*elf_load_sym_32(const t_elf_map_32 *map, const Elf32_Sym *symbol)
{
	t_list		*list_elem;
	t_elf_sym	*new_sym;

	list_elem = malloc(sizeof(*list_elem) + sizeof(*new_sym));
	if (list_elem != NULL)
	{	new_sym = (t_elf_sym*)(list_elem + 1);
		new_sym->symbol = symbol;
		new_sym->name = map->str + symbol->st_name;
		new_sym->identifier = elf_sym_type_32(map, symbol);
		if (new_sym->identifier != ELF_SHID_COMMON
		&& new_sym->identifier != ELF_SHID_COMMON + ('A' - 'a'))
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
 * @param	t_elf_opt	ELF symbol filters.	
 * @return	t_elf_err	Zero or error code.
 */
t_elf_err	elf_load_syms_64(t_list	**dest, const t_elf_map_64 *map,
	t_elf_opt options)
{
	t_list			*elem;
	Elf64_Half		i;
	int				err;

	err = ELF_EOK;
	*dest = NULL;
	i = 0;
	while (err == ELF_EOK && i < map->sym_count)
	{
		err = elf_sym_validate_64(map, map->sym + i);
		if (err == ELF_EOK && *(map->str + map->sym[i].st_name) != '\0')
		{
			if (options & ELF_ODEBUG || ELF64_ST_TYPE(map->sym[i].st_info) != STT_FILE)
			{
				elem = elf_load_sym_64(map, map->sym + i);
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
 * @brief	Validate, load and identify 32 bit ELF symbols into a linked-list.
 * 
 * @param	dest		Destination pointer for the head of the linked list.
 * @param	map			Parsed 32 bit ELF map.
 * @return	t_elf_err	Zero or error code.
 */
t_elf_err	elf_load_syms_32(t_list	**dest, const t_elf_map_32 *map,
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
		if (err == ELF_EOK && *(map->str + map->sym[i].st_name) != '\0')
		{
			if (options & ELF_ODEBUG || ELF32_ST_TYPE(map->sym->st_info) != STT_FILE)
			{
				elem = elf_load_sym_32(map, map->sym + i);
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
static void	elf_print_sym(void *data, unsigned value_width)
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
void		elf_print_sym_64(void *data)
{
	elf_print_sym(data, sizeof(Elf64_Addr) * 2);
}

/**
 * @brief	Print a 32 bit ELF symbol's value, identifier and name.
 * 
 * @param	data		Pointer to the ELF symbol.
 */
void		elf_print_sym_32(void *data)
{
	elf_print_sym(data, sizeof(Elf32_Addr) * 2);
}

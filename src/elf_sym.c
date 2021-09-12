#include <stdlib.h>

#include <elf_sym.h>

#include <inttypes.h>

char	is_weak_64(const t_elf_map_64 *map, const Elf64_Sym *symbol)
{
	char	identifier = ELF_SYMID_UNKNOWN;

	(void) map;
	if (ELF64_ST_BIND(symbol->st_info) & STB_WEAK)
		identifier = ELF_SYMID_WEAK;

	return identifier;
}


char	elf_sym_locate(const t_elf_map_64 *map, const Elf64_Sym *symbol)
{
	char	identifier = '?';

	(void) map;
	if (symbol->st_shndx == SHN_ABS)
		identifier = ELF_SHID_ABS;
	else if (symbol->st_shndx == SHN_COMMON)
		identifier = ELF_SHID_COMMON;
	else
		identifier = map->shid[symbol->st_shndx];
	// TODO: Handle overflow by using sh_count!
	return identifier;
}

char	elf_sym_type_64(const t_elf_map_64 *map, const Elf64_Sym *symbol)
{
	char	identifier;

	identifier = elf_sym_locate(map, symbol);

	if (symbol->st_value == 0)
		identifier = ELF_SYMID_UNDEFINED;
	if (ft_islower(identifier) && ELF64_ST_BIND(symbol->st_info) == STB_GLOBAL)
		identifier += 'A' - 'a';
	return identifier;
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
	new_sym->identifier = elf_sym_type_64(map, symbol);
	list_elem->content = new_sym;
	list_elem->next = NULL;
	return (list_elem);
}

t_list	*elf_load_syms_64(const t_elf_map_64 *map)
{
	t_list			*symbols;
	t_list			*elem;
	Elf64_Half		i;

	symbols = NULL;
	i = 0;
	while (i < map->sym_count)
	{
		if (*(map->str + map->sym[i].st_name))
		{
			if (!(elem = elf_load_sym_64(map, map->sym + i)))
			{
				ft_lstclear(&symbols, NULL);
				return (NULL);
			}
			//ft_printf("Loading %d->%s...\n", map->sym[i].st_shndx, map->str + map->sym[i].st_name);
			ft_lstadd_front(&symbols, elem);
		}
		i++;
	}
	return (symbols);
}

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


	// TODO: Toggle this predicate using '-A/-o/--print-file-name' flag
	if (ELF32_ST_TYPE(sym->symbol->st_info) != STT_FILE)
	{
		if (sym->identifier == ELF_SYMID_UNDEFINED)
			ft_printf("%16s %c %s\n", "", sym->identifier, sym->name);
		else
			ft_printf("%016"PRIx64" %c %s\n",
			sym->symbol->st_value,
			sym->identifier,
			sym->name);
	}

}
